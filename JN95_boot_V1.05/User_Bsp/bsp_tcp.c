/**********************************************************************************************************
*
*	ģ������ : ����BOOT����TCPЭ�鴦��
*	�ļ����� : bsp_tcp.c
*	��    �� : V1.0
*	˵    �� : ����CRC16����
*	��	 �ߣ�  wang liang
*	��	 ��:  2021.10.1

**********************************************************************************************************/

#include "bsp_tcp.h"
#include "bsp_CRC16.h"
#include "string.h"
#include "bsp_eeprom.h"
#include "main.h"
#include "bsp_dwt.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
uint8_t CMD[TCP_RX_BUFFSIZE] = {0}; //���ݽ���1������
uint8_t tcp_recvbuf[TCP_RX_BUFFSIZE];        //TCP client ��������2��������
uint8_t tcp_sentbuf[TCP_TX_BUFFSIZE];        //TCP_client �������ݻ�����
uint16_t Num_Recived;     //�������ݳ���
uint8_t  TCP_SERVER_FLAG;  //���ձ�־
uint8_t Status_Recive = 0;
uint32_t Write_ADDR = ADDR_FLASH_SECTOR_5;   //д��ʼ��ַ

/*
*********************************************************************************************************
*   �� �� ��: W5500_TCP_Server
*   ����˵��: ���ڷ�����״̬��
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void W5500_TCP_Server(void)
{
    uint8_t Socket_SR;
    uint16_t uwCmdLen;     //�������ݳ���


    Socket_SR = getSn_SR(Socket_0);  // ��ȡSocket_0��״̬
    switch(Socket_SR)
    {
        case SOCK_CLOSED:
            W5500_socket(Socket_0, Sn_MR_TCP, W5500_localport , Sn_MR_ND);       /*��socket0,�󶨶˿�5000*/
            break;
        case SOCK_INIT:
            W5500_listen(Socket_0);                                    /*��ʼ����*/
            break;
        case SOCK_ESTABLISHED:

            //��ȡ����
            if(getSn_IR(Socket_0) & Sn_IR_CON)
            {
                setSn_IR(Socket_0, Sn_IR_CON);                                                            /*��������жϱ�־λ*/
            }
            uwCmdLen = getSn_RX_RSR(Socket_0);                                                       /*��ȡ�������ݳ���*/
            if(uwCmdLen > 0)
            {
                W5500_recv(Socket_0, CMD, uwCmdLen);
                Status_Recive = 0x01; //���ճɹ�
                Num_Recived = uwCmdLen;
            }

            break;
        case SOCK_CLOSE_WAIT:
            W5500_disconnect(Socket_0);    //�Ͽ�����
            W5500_close(Socket_0);
            break;
        default
                :
            break;
    }
}


/*
*********************************************************************************************************
*   �� �� ��: Socket_Receive
*   ����˵��: ���ڽ������ݻ��濽������
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/

void Socket_Receive(void)
{
    if((!(TCP_SERVER_FLAG & 0x40)) && (Status_Recive == 0x01))
    {
        memset(tcp_recvbuf, 0, TCP_RX_BUFFSIZE);  //�������

        memcpy(tcp_recvbuf, CMD, Num_Recived);     //��������

        TCP_SERVER_FLAG |= 1 << 6;              //��ǽ��ܵ�������

        Status_Recive = 0; //������ձ�־
    }

}

/*
*********************************************************************************************************
*	�� �� ��: GetSector
*	����˵��: ��ȡ������ַ
*	��    ��: �����ַ
*	�� �� ֵ: ������ַ
*********************************************************************************************************
*/	
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;  
  }
  else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
  {
    sector = FLASH_SECTOR_11;  
  }
  else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
  {
    sector = FLASH_SECTOR_12;  
  }
  else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
  {
    sector = FLASH_SECTOR_13;  
  }
  else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
  {
    sector = FLASH_SECTOR_14;  
  }
  else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
  {
    sector = FLASH_SECTOR_15;  
  }
  else if((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
  {
    sector = FLASH_SECTOR_16;  
  }
  else if((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
  {
    sector = FLASH_SECTOR_17;  
  }
  else if((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
  {
    sector = FLASH_SECTOR_18;  
  }
  else if((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
  {
    sector = FLASH_SECTOR_19;  
  }
  else if((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
  {
    sector = FLASH_SECTOR_20;  
  } 
  else if((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
  {
    sector = FLASH_SECTOR_21;  
  }
  else if((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
  {
    sector = FLASH_SECTOR_22;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
  {
    sector = FLASH_SECTOR_23;  
  }

  return sector;
}
/*
*********************************************************************************************************
*	�� �� ��: GetSectorSize
*	����˵��: ��ȡ������С��λB
*	��    ��: 
*	�� �� ֵ: 
*********************************************************************************************************
*/
static uint32_t GetSectorSize(uint32_t Sector)
{
  uint32_t sectorsize = 0x00;
  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) ||\
     (Sector == FLASH_SECTOR_3) || (Sector == FLASH_SECTOR_12) || (Sector == FLASH_SECTOR_13) ||\
     (Sector == FLASH_SECTOR_14) || (Sector == FLASH_SECTOR_15))
  {
    sectorsize = 16 * 1024;
  }
  else if((Sector == FLASH_SECTOR_4) || (Sector == FLASH_SECTOR_16))
  {
    sectorsize = 64 * 1024;
  }
  else
  {
    sectorsize = 128 * 1024;
  }  
  return sectorsize;
}
/*
*********************************************************************************************************
*	�� �� ��: Flash_In_Erase
*	����˵��: Flash����
*	��    ��: ��ʼ��ַ��������ַ
*	�� �� ֵ: ״̬
*********************************************************************************************************
*/
  uint32_t FirstSector = 0, NbOfSectors = 0, Address = 0;
  uint32_t SectorError = 0;
  uint8_t Status=0;
  uint8_t Flash_In_Erase(uint32_t StartAddress,uint32_t EndAddress)
{

	/*Variable used for Erase procedure*/
  static FLASH_EraseInitTypeDef EraseInitStruct;
	
	/* Unlock the Flash to enable the flash control register access *************/ 
  HAL_FLASH_Unlock();
	/* Erase the user Flash area
  (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Get the 1st sector to erase */
  FirstSector = GetSector(StartAddress);
  /* Get the number of sector to erase from 1st sector*/
  NbOfSectors = GetSector(EndAddress) - FirstSector + 1;//����ĩ��ַ��ȥ��ʼ��ַ

  /* Fill EraseInit structure*/
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInitStruct.Banks  =FLASH_BANK_1;
  EraseInitStruct.Sector = FirstSector;
  EraseInitStruct.NbSectors = NbOfSectors;//��С
  __HAL_FLASH_CLEAR_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR)); 
  if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  { 
    Status=1;
  }
 
	  /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
     you have to make sure that these data are rewritten before they are accessed during code
     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
     DCRST and ICRST bits in the FLASH_CR register. */
  __HAL_FLASH_DATA_CACHE_DISABLE();
  __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

  __HAL_FLASH_DATA_CACHE_RESET();
  __HAL_FLASH_INSTRUCTION_CACHE_RESET();

  __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
  __HAL_FLASH_DATA_CACHE_ENABLE();

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock(); 
	
	return Status;
}

/*
*********************************************************************************************************
*	�� �� ��: Flash_In_Write_DoubleWORD
*	����˵��: Flashһ��д��8���ֽ�
*	��    ��: size ���ֽ��� >=8
*	�� �� ֵ: ״̬
*********************************************************************************************************
*/
uint8_t Flash_In_Write_DoubleWORD(uint32_t addr,uint8_t *buf,uint32_t size)
{
	uint32_t t;
	uint32_t Buf_temp=0;
	uint32_t Address = addr;
	uint8_t *dfu=buf;
	uint8_t Status=0;
	
	/* Unlock the Flash to enable the flash control register access *************/ 
  HAL_FLASH_Unlock();


  /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
	
	for(t=0;t<size;t+=4)
	{						   
		Buf_temp|=(uint32_t)(dfu[3]<<24);   
		Buf_temp|=(uint32_t)(dfu[2]<<16);    
		Buf_temp|=(uint32_t)(dfu[1]<<8);
		Buf_temp|=(uint32_t)dfu[0];	  
		dfu+=4;//ƫ��4���ֽ�  		
		if (HAL_FLASH_Program(TYPEPROGRAM_WORD, Address, Buf_temp) == HAL_OK)
		{
			Address += 4;
		}
		else
		{ 
      Status=1;
		}
		Buf_temp=0;
	} 
  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();  
	
	return Status;
}

/*
*********************************************************************************************************
*   �� �� ��: TCP_RX_Data_Analysis
*   ����˵��: ���ڽ������ݴ���
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t Pack_NO = 0;
void TCP_RX_Data_Analysis(void)
{
    
    uint8_t bootflag1=0xA0;
    uint8_t bootflag2=0x50;    
    uint16_t Data_Crc = 0xFFFF;
    uint16_t Buf_Crc = 0x0000;
    uint16_t TBuf_Crc = 0x0000;
    uint16_t Data_Len = 0;
    uint16_t Data_Start = 0;
    uint16_t Data_End = 0;
    uint16_t RX_Pack_NO = 0;
    static uint8_t  Data_Buf[TCP_TX_BUFFSIZE];
    uint32_t  Addr_Start = 0;
    uint32_t  Addr_End = 0;
    memset(Data_Buf, 0, TCP_TX_BUFFSIZE);
    if(TCP_SERVER_FLAG & 0x40) //������յ�������
    {
        memset(tcp_sentbuf, 0, TCP_TX_BUFFSIZE);      //���ͻ�������
        Data_Len = (uint16_t)(tcp_recvbuf[3] << 8) + tcp_recvbuf[2];
        Data_Start = (uint16_t)(tcp_recvbuf[1] << 8) + tcp_recvbuf[0];
        RX_Pack_NO = (uint16_t)(tcp_recvbuf[8] << 8) + tcp_recvbuf[7];
        Data_End = (uint16_t)(tcp_recvbuf[Data_Len + 3] << 8) + tcp_recvbuf[Data_Len + 2];
        Buf_Crc = (uint16_t)(tcp_recvbuf[Data_Len + 1] << 8) + tcp_recvbuf[Data_Len];
        Data_Crc = CRC16N(&tcp_recvbuf[2], Data_Len - 2);

        if((Data_Start == 0x5A5A) && (Data_End == 0xA5A5)) //֡ͷ֡β
        {
            tcp_sentbuf[0] = 0x5A;
            tcp_sentbuf[1] = 0x5A;
            switch(tcp_recvbuf[5])
            {
                case Upgrade_CMD:
                {
                    //1
                    switch(tcp_recvbuf[6])
                    {
                            //2
                        case Query_Boot_CMD:
                        {
                            if(Data_Len == 7)
                            {
                                if(Data_Crc == Buf_Crc)
                                {
                                    tcp_sentbuf[7] = 0x01; //Boot��־OK
                                }
                                else
                                { tcp_sentbuf[7] = 0x02; }
                            }
                            else
                            { tcp_sentbuf[7] = 0x03; }
                            tcp_sentbuf[2] = 0x08;
                            tcp_sentbuf[3] = 0x00;
                            tcp_sentbuf[4] = tcp_recvbuf[4];
                            tcp_sentbuf[5] = 0x0E;
                            tcp_sentbuf[6] = Query_Boot_CMD;
                            TBuf_Crc = CRC16N(&tcp_sentbuf[2], 6);
                            memcpy(&tcp_sentbuf[8], &TBuf_Crc, 2);
                            tcp_sentbuf[10] = 0xA5;
                            tcp_sentbuf[11] = 0xA5;
                            //��ӷ���
                            TCP_SERVER_FLAG &= ~(1 << 6);   //��������ѱ�����
                            W5500_send(0, tcp_sentbuf, 12);

                            break;
                        }
                        case Flash_Erase_CMD:
                        {
                            if(Data_Len == 15)
                            {
                                if(Data_Crc == Buf_Crc)
                                {
                                    Write_ADDR = ADDR_FLASH_SECTOR_5;
                                    memcpy(&Addr_Start, &tcp_recvbuf[7], 4);
                                    memcpy(&Addr_End, &tcp_recvbuf[11], 4);

                                           __set_PRIMASK(1);  /* ��ֹȫ���ж� */
                             
                                    if(Flash_In_Erase(ADDR_FLASH_SECTOR_5, ADDR_FLASH_SECTOR_6) == 0) //���11��Ŀǰֻ����2�������ϼ�516K
                                    {
                                        tcp_sentbuf[7] = 0x01;
                                        Pack_NO = 0;
                                    }
                                    else
                                    { tcp_sentbuf[7] = 0x05; }
                             
                                          __set_PRIMASK(0);  /* ʹ��ȫ���ж� */

                                }
                                else
                                { tcp_sentbuf[7] = 0x02; }
                            }
                            else
                            { tcp_sentbuf[7] = 0x03; }
                            tcp_sentbuf[2] = 0x08;
                            tcp_sentbuf[3] = 0x00;
                            tcp_sentbuf[4] = tcp_recvbuf[4];
                            tcp_sentbuf[5] = 0x0E;
                            tcp_sentbuf[6] = Flash_Erase_CMD;
                            TBuf_Crc = CRC16N(&tcp_sentbuf[2], 6);
                            memcpy(&tcp_sentbuf[8], &TBuf_Crc, 2);
                            tcp_sentbuf[10] = 0xA5;
                            tcp_sentbuf[11] = 0xA5;
                            //��ӷ���
                            TCP_SERVER_FLAG &= ~(1 << 6);   //��������ѱ�����

                            W5500_send(0, tcp_sentbuf, 12);

                        }
                        break;

                        case Flash_Write_CMD:
                        {
                            if(Data_Len == 1033)
                            {
                                if(Data_Crc == Buf_Crc)
                                {
                                    if(RX_Pack_NO == Pack_NO)
                                    {
                                        tcp_sentbuf[7] = 0x01;
                                        memcpy(&Data_Buf[0], &tcp_recvbuf[9], 1024);
                                        __set_PRIMASK(1);  /* ��ֹȫ���ж� */
                                       //  HAL_NVIC_DisableIRQ(TIM4_IRQn);
                                        if(Flash_In_Write_DoubleWORD(Write_ADDR, &Data_Buf[0], 1024) != 0)
                                        {
                                            tcp_sentbuf[7] = 0x05;
                                        }
                                        else
                                        {
                                            Write_ADDR = Write_ADDR + 1024; //��ַ����
                                            Pack_NO++;
                                        }
                                        __set_PRIMASK(0);  /* ʹ��ȫ���ж� */
                                       //  HAL_NVIC_EnableIRQ(TIM4_IRQn);
                                    }
                                    else
                                    { tcp_sentbuf[7] = 0x04; }
                                }
                                else
                                { tcp_sentbuf[7] = 0x02; }
                            }
                            else
                            { tcp_sentbuf[7] = 0x03; }
                            tcp_sentbuf[2] = 0x08;
                            tcp_sentbuf[3] = 0x00;
                            tcp_sentbuf[4] = tcp_recvbuf[4];
                            tcp_sentbuf[5] = 0x0E;
                            tcp_sentbuf[6] = Flash_Write_CMD;
                            TBuf_Crc = CRC16N(&tcp_sentbuf[2], 6);
                            memcpy(&tcp_sentbuf[8], &TBuf_Crc, 2);
                            tcp_sentbuf[10] = 0xA5;
                            tcp_sentbuf[11] = 0xA5;
                            //��ӷ���
                            TCP_SERVER_FLAG &= ~(1 << 6);   //��������ѱ�����

                            W5500_send(0, tcp_sentbuf, 12);

                        }
                        break;
                        case JMP_APP_CMD:
                        {
                            if(Data_Len == 7)
                            {
                                if(Data_Crc == Buf_Crc)
                                {
                                    tcp_sentbuf[7] = 0x01;
                                    tcp_sentbuf[2] = 0x08;
                                    tcp_sentbuf[3] = 0x00;
                                    tcp_sentbuf[4] = tcp_recvbuf[4];
                                    tcp_sentbuf[5] = 0x0E;
                                    tcp_sentbuf[6] = JMP_APP_CMD;
                                    TBuf_Crc = CRC16N(&tcp_sentbuf[2], 6);
                                    memcpy(&tcp_sentbuf[8], &TBuf_Crc, 2);
                                    tcp_sentbuf[10] = 0xA5;
                                    tcp_sentbuf[11] = 0xA5;
                                    //��ӷ���
                                    TCP_SERVER_FLAG &= ~(1 << 6);   //��������ѱ�����


                                    W5500_send(0, tcp_sentbuf, 12);

                                    ee_WriteBytes(&bootflag1,Boot_Flag_addr - 1,1 );  //0xA0
                                    bsp_DelayMS(1);
                                    ee_WriteBytes(&bootflag2,Boot_Flag_addr, 1);      //0x50
                                    bsp_DelayMS(1);
                                    Boot_JMP_APP(); 
                                }
                                else
                                { tcp_sentbuf[7] = 0x02; }
                            }
                            else
                            { tcp_sentbuf[7] = 0x03; }

                            tcp_sentbuf[2] = 0x08;
                            tcp_sentbuf[3] = 0x00;
                            tcp_sentbuf[4] = tcp_recvbuf[4];
                            tcp_sentbuf[5] = 0x0E;
                            tcp_sentbuf[6] = JMP_APP_CMD;
                            TBuf_Crc = CRC16N(&tcp_sentbuf[2], 6);
                            memcpy(&tcp_sentbuf[8], &TBuf_Crc, 2);
                            tcp_sentbuf[10] = 0xA5;
                            tcp_sentbuf[11] = 0xA5;
                            //��ӷ���
                            TCP_SERVER_FLAG &= ~(1 << 6);   //��������ѱ�����

                            W5500_send(0, tcp_sentbuf, 12);
                            bsp_DelayMS(1);
  //                          NVIC_SystemReset();
                        }
                        break;
                        default
                                :
                            break;
                    }
                    break;
                }
                default
                        :
                    break;
            }
        }
        TCP_SERVER_FLAG &= ~(1 << 6);   //��������ѱ�����
    }
}
