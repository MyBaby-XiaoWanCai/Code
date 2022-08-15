/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: FLASH_BSP.c
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "F28x_Project.h"
//ȫ�ֱ�������
//#pragma DATA_SECTION(SectorImage,".mySectorImage");
         //  unsigned char SectorImage[4096];          //����Flash�޸Ĳ���ʱ�����ӦSector�ľ���
     INTERFACE_SPI_FLASH Interface_SPI_Flash[NUM_SERVICE_FLASH];
STATUS_SPI_FLASH_SERVICE Flash_Service;
//��������


//��������
//*********************************************************************************
//Description: SPI�ӿڳ�ʼ������
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void InitFlash_BSP(void)
{
    Uint16 Cnt=0;
    Uint32 temp1,temp2;

    temp1=(Uint32)&Interface_SPI_Flash;
    temp2=sizeof(INTERFACE_SPI_FLASH);

    InitSpiaGpio();

    SpiaRegs.SPICCR.bit.SPISWRESET=0;
    SpiaRegs.SPICCR.all=0x07;
    SpiaRegs.SPICCR.bit.SPILBK=0;
    SpiaRegs.SPICTL.all=0x0e;
    SpiaRegs.SPIBRR.bit.SPI_BIT_RATE=0x0F;
    SpiaRegs.SPIFFTX.all=0xc008;
    SpiaRegs.SPIFFRX.all=0x0008;
    SpiaRegs.SPIFFCT.all=0x01;
    SpiaRegs.SPIPRI.all=0x0010;
    SpiaRegs.SPICCR.bit.SPISWRESET=1;
    SpiaRegs.SPIFFTX.bit.TXFIFO=1;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET=1;

    for(Cnt=0;Cnt<NUM_SERVICE_FLASH;Cnt++)
    {
        Interface_SPI_Flash[Cnt].Len_Buff=LEN_SPI_FLASH_BUFF;
        Interface_SPI_Flash[Cnt].Num_Overtime=100;
        Interface_SPI_Flash[Cnt].Num_Bytes=0;
        Interface_SPI_Flash[Cnt].Req_RW=REQ_SPI_FLASH_IDLE;
        Interface_SPI_Flash[Cnt].Result_RW=SPI_FLASH_IDLE;

        if(Cnt==NUM_SERVICE_FLASH-1)
        {Interface_SPI_Flash[Cnt].Ptr_Next=(INTERFACE_SPI_FLASH *)temp1;}
        else
        {Interface_SPI_Flash[Cnt].Ptr_Next=(INTERFACE_SPI_FLASH *)(temp1+(Cnt+1)*temp2);}
    }

    Flash_Service.Cnt_Bytes=0;
    Flash_Service.Cnt_Error=0;
    Flash_Service.Cnt_Overtime=0;
    Flash_Service.Ptr=(INTERFACE_SPI_FLASH *)temp1;
    Flash_Service.Serv_Ready=1;
    Flash_Service.State=1;
}

//*********************************************************************************
//Description: ��SPI_Flash_Service����ģ���ύ1��Flashд�������� д�����ĵ�ַ������������һ��Sector��
//Parameters : Ptr  -ָ�����������ķ���ӿ�
//             Addr -���ݴ�д��λ�õ���ʼ��ַ��Num����1ʱ��ַΪAddr��ʼ��Num����ַ
//             Num  -��д�����ݵ�����
//             Ptr_Data -��д������ָ��
//Return     : TRUE -�����ύ�ɹ�
//             False-�����ύʧ��
//*********************************************************************************
Uint16 SPI_Flash_Write(Uint16 Index,Uint32 Addr,Uint16 Num,unsigned char *Ptr_Data)
{
    INTERFACE_SPI_FLASH *Ptr;
    Uint32 temp1,temp2;
    temp1=(Uint32)&Interface_SPI_Flash[0];
    temp2=sizeof(INTERFACE_SPI_FLASH);

    Ptr=(INTERFACE_SPI_FLASH *)(temp1+temp2*Index);

    Uint16 Cnt=0;
    if(Ptr->Result_RW!=SPI_FLASH_IDLE)
    {return(FALSE);}

    if(Ptr->Req_RW!=REQ_SPI_FLASH_IDLE)
    {return(FALSE);}

    if(Num>Ptr->Len_Buff)
    {return(FALSE);}

    Ptr->Addr_RW=Addr;

    Ptr->Offset_Sector=Addr>>12;
    Ptr->Offset_In_Sector=Addr&0x00000FFF;
    Ptr->Num_Bytes=Num;

    for(Cnt=0;Cnt<Num;Cnt++)
    {
        Ptr->Data[Cnt]=(*Ptr_Data++)&0x00FF;
    }

    Ptr->Req_RW=REQ_SPI_FLASH_WRITE;

    return(TRUE);
}

//*********************************************************************************
//Description: ��ѯ��Ӧ��SPI_Flash_Service����д�����Ƿ����
//Parameters : Ptr  -ָ�����ѯ�ķ���ӿ�
//Return     : BUSY -д�������æ״̬
//             TRUE -д��ɹ�
//             False-д��ʧ��
//*********************************************************************************
Uint16 Qurey_SPI_Flash_Write(Uint16 Index)
{
    INTERFACE_SPI_FLASH *Ptr;
    Uint32 temp1,temp2;
    temp1=(Uint32)&Interface_SPI_Flash[0];
    temp2=sizeof(INTERFACE_SPI_FLASH);

    Ptr=(INTERFACE_SPI_FLASH *)(temp1+temp2*Index);

    if(Ptr->Req_RW==REQ_SPI_FLASH_IDLE)
    {
        if(Ptr->Result_RW==SPI_FLASH_WRITE_OK)
        {   return(TRUE);   }
        else
        {   return(FALSE);  }
    }
    else
    {   return(BUSY);   }
}

//*********************************************************************************
//Description: ��SPI_Flash_Service����ģ���ύ1��Flash���������󣬶�ȡ��ɺ����ݽ������Ptrָ������ݻ�����Data
//Parameters : Ptr  -ָ�����������ķ���ӿ�
//             Addr -���ݴ�����λ�õ���ʼ��ַ��Num����1ʱ����ΪAddr��ʼ��Num��
//             Num  -���������ݵ�����
//Return     : TRUE -�����ύ�ɹ�
//             False-�����ύʧ��
//*********************************************************************************
Uint16 SPI_Flash_Read(Uint16 Index,Uint32 Addr,Uint16 Num)
{
    INTERFACE_SPI_FLASH *Ptr;
    Uint32 temp1,temp2;
    temp1=(Uint32)&Interface_SPI_Flash[0];
    temp2=sizeof(INTERFACE_SPI_FLASH);

    Ptr=(INTERFACE_SPI_FLASH *)(temp1+temp2*Index);

    if(Ptr->Result_RW!=SPI_FLASH_IDLE)
    {return(FALSE);}

    if(Ptr->Req_RW!=REQ_SPI_FLASH_IDLE)
    {return(FALSE);}

    if(Num>Ptr->Len_Buff)
    {return(FALSE);}

    Ptr->Addr_RW=Addr;

    Ptr->Offset_Sector=Addr>>12;
    Ptr->Offset_In_Sector=Addr&0x00000FFF;
    if(Num>Ptr->Len_Buff)
    {
        Num= Ptr->Len_Buff;
    }
    Ptr->Num_Bytes=Num;

    Ptr->Req_RW=REQ_SPI_FLASH_READ;

    return(TRUE);
}

//*********************************************************************************
//Description: ��ѯ��Ӧ��SPI_Flash_Service����������Ƿ����
//Parameters : Ptr  -ָ�����ѯ�ķ���ӿ�
//Return     : BUSY -��������æ״̬
//             TRUE -���ɹ�
//             False-��ʧ��
//*********************************************************************************
Uint16 Qurey_SPI_Flash_Read(Uint16 Index)
{
    INTERFACE_SPI_FLASH *Ptr;
    Uint32 temp1,temp2;
    temp1=(Uint32)&Interface_SPI_Flash[0];
    temp2=sizeof(INTERFACE_SPI_FLASH);

    Ptr=(INTERFACE_SPI_FLASH *)(temp1+temp2*Index);

    if(Ptr->Req_RW==REQ_SPI_FLASH_IDLE)
    {
        if(Ptr->Result_RW==SPI_FLASH_READ_OK)
        {   return(TRUE);   }
        else
        {   return(FALSE);  }
    }
    else
    {   return(BUSY);   }
}


//*********************************************************************************
//Description: ��д��ɺ�λFLASH����ӿڵ�Ĭ��״̬
//Parameters : Ptr  -ָ�����λ�ķ���ӿ�
//Return     : TRUE -��λ�ɹ�
//             FALSE-��λʧ��
//*********************************************************************************
Uint16 Reset_SPI_FLASH_Service(Uint16 Index)
{
    INTERFACE_SPI_FLASH *Ptr;
    Uint32 temp1,temp2;
    temp1=(Uint32)&Interface_SPI_Flash[0];
    temp2=sizeof(INTERFACE_SPI_FLASH);
    Uint16 Cnt;

    Ptr=(INTERFACE_SPI_FLASH *)(temp1+temp2*Index);

    if(Ptr->Result_RW==SPI_FLASH_WORKING)
    {return(FALSE);}
    else
    {
        Ptr->Req_RW= REQ_SPI_FLASH_IDLE;
        Ptr->Result_RW=SPI_FLASH_IDLE;
        Ptr->Addr_RW=0;
        Ptr->Offset_In_Sector=0;
        Ptr->Offset_Sector=0;
        Ptr->Num_Bytes=0;
        for(Cnt=0;Cnt<LEN_SPI_FLASH_BUFF;Cnt++)
        {
            Ptr->Data[Cnt]=0;
        }

        return(TRUE);
    }
}

//*********************************************************************************
//Description: SPI�ӿ�Flash��д����״̬��
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
//#pragma CODE_SECTION(SPI_Flash_Service,"ramfuncs");
void SPI_Flash_Service(void)
{
    switch(Flash_Service.State)
    {
        case 1:{
                 if((Flash_Service.Ptr->Result_RW==SPI_FLASH_IDLE)&&(Flash_Service.Ptr->Req_RW!=REQ_SPI_FLASH_IDLE))
                 {
                     Flash_Service.Cnt_Bytes=0;
                     Flash_Service.Cnt_Page_Write=0;
                     Flash_Service.Num_Last_RW=0;
                     Flash_Service.Cnt_Overtime=0;
                     if(Flash_Service.Ptr->Req_RW==REQ_SPI_FLASH_READ)
                     {   //FLASH������
                         Flash_Service.State=2;
                         Flash_Service.Addr_RW_Now=Flash_Service.Ptr->Addr_RW;
                     }
                     else if(Flash_Service.Ptr->Req_RW==REQ_SPI_FLASH_WRITE)
                     {   //FLASHд����
                         Flash_Service.State=12;
                         Flash_Service.Addr_RW_Now=Flash_Service.Ptr->Offset_Sector<<12;
                     }
                     else
                     {
                         Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                         Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                     }
                 }
                 else
                 {
                     Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                 }
             break;}

        case 2:{SpiaRegs.SPIFFRX.bit.RXFIFORESET=0;
                SpiaRegs.SPIFFRX.bit.RXFIFORESET=1;
                CLEAR_FLASH_CS;
                SpiaRegs.SPITXBUF=0x0500;
                SpiaRegs.SPITXBUF=0x0000;
                Flash_Service.State=3;
                break;}

        case 3:{SET_FLASH_CS;
                if(SpiaRegs.SPIFFRX.bit.RXFFST==2)
                {
                    Uint16 Temp;
                    Temp=SpiaRegs.SPIRXBUF;
                    Temp=SpiaRegs.SPIRXBUF;
                    if((Temp&0x01)==0)
                    {   //Flash���ڿ���״̬
                        CLEAR_FLASH_CS;
                        Flash_Service.State=4;
                        SpiaRegs.SPITXBUF=0x0300;
                        SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now>>8)&0x0000FF00;
                        SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now   )&0x0000FF00;
                        SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now<<8)&0x0000FF00;

                        Flash_Service.Num_Last_RW=0;
                        if(Flash_Service.Ptr->Num_Bytes<=Flash_Service.Cnt_Bytes+12)
                        {
                            while(Flash_Service.Num_Last_RW+Flash_Service.Cnt_Bytes<Flash_Service.Ptr->Num_Bytes)
                            {
                                SpiaRegs.SPITXBUF=0xAA00;
                                Flash_Service.Num_Last_RW++;
                            }
                        }
                        else
                        {
                            while(Flash_Service.Num_Last_RW<12)
                            {
                                SpiaRegs.SPITXBUF=0xAA00;
                                Flash_Service.Num_Last_RW++;
                            }
                        }
                    }
                    else
                    {
                        if(++Flash_Service.Cnt_Overtime>=Flash_Service.Ptr->Num_Overtime)
                        {
                            Flash_Service.Cnt_Overtime=0;
                            Flash_Service.Cnt_Bytes=0;
                            Flash_Service.Ptr->Result_RW=SPI_FLASH_READ_ERR;
                            Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                            Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                            Flash_Service.State=1;
                        }
                        else
                        {   //Flash���ڷǿ���״̬���ٴζ�ȡ״̬
                            Flash_Service.State=2;
                        }
                    }
                }
                else
                {
                    if(++Flash_Service.Cnt_Overtime>=Flash_Service.Ptr->Num_Overtime)
                    {
                        Flash_Service.Cnt_Overtime=0;
                        Flash_Service.Cnt_Bytes=0;
                        Flash_Service.Ptr->Result_RW=SPI_FLASH_READ_ERR;
                        Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                        Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                        Flash_Service.State=1;
                        SET_FLASH_CS;
                    }
                    else
                    {   //���յ����ֽ����������¶�ȡFLASH��״̬
                        Flash_Service.State=2;
                    }
                }
                break;}

     case 4:{SET_FLASH_CS;
             if(SpiaRegs.SPIFFRX.bit.RXFFST==Flash_Service.Num_Last_RW+4)
             {   //���յ��ֽ�����ȷ
                 Uint16 Temp;
                 Temp=SpiaRegs.SPIRXBUF;
                 Temp=SpiaRegs.SPIRXBUF;
                 Temp=SpiaRegs.SPIRXBUF;
                 Temp=SpiaRegs.SPIRXBUF;

                 Flash_Service.Cnt_Overtime=0;

                 Flash_Service.Addr_RW_Now=Flash_Service.Addr_RW_Now+Flash_Service.Num_Last_RW;

                 while((Flash_Service.Num_Last_RW>0)&&(Flash_Service.Cnt_Bytes<Flash_Service.Ptr->Len_Buff))
                 {
                     Flash_Service.Ptr->Data[Flash_Service.Cnt_Bytes++]=SpiaRegs.SPIRXBUF&0x00FF;
                     Flash_Service.Num_Last_RW--;
                 }

                 if((Flash_Service.Cnt_Bytes>=Flash_Service.Ptr->Num_Bytes)||(Flash_Service.Cnt_Bytes>=Flash_Service.Ptr->Len_Buff))
                 {
                     Flash_Service.Ptr->Result_RW=SPI_FLASH_READ_OK;
                     Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                     Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                     Flash_Service.State=1;
                 }
                 else
                 {
                     Flash_Service.State=2;
                 }
             }
             else
             {
                 if(++Flash_Service.Cnt_Overtime>=Flash_Service.Ptr->Num_Overtime)
                 {
                     Flash_Service.Cnt_Bytes=0;
                     Flash_Service.Cnt_Overtime=0;
                     Flash_Service.Ptr->Result_RW=SPI_FLASH_READ_ERR;
                     Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                     Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                     Flash_Service.State=1;
                 }
                 else
                 {
                     Flash_Service.State=2;
                 }
             }
             break;}

     case 12:{SpiaRegs.SPIFFRX.bit.RXFIFORESET=0;
             SpiaRegs.SPIFFRX.bit.RXFIFORESET=1;
             CLEAR_FLASH_CS;
             SpiaRegs.SPITXBUF=0x0500;
             SpiaRegs.SPITXBUF=0x0000;
             Flash_Service.State=13;
             break;}

     case 13:{SET_FLASH_CS;
             if(SpiaRegs.SPIFFRX.bit.RXFFST==2)
             {
                 Uint16 Temp;
                 Temp=SpiaRegs.SPIRXBUF;
                 Temp=SpiaRegs.SPIRXBUF;
                 if((Temp&0x01)==0)
                 {   //Flash���ڿ���״̬
                     CLEAR_FLASH_CS;
                     Flash_Service.State=14;
                     SpiaRegs.SPITXBUF=0x0300;
                     SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now>>8)&0x0000FF00;
                     SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now   )&0x0000FF00;
                     SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now<<8)&0x0000FF00;

                     Flash_Service.Num_Last_RW=0;
                     if(4096<=Flash_Service.Cnt_Bytes+12)
                     {
                         while(Flash_Service.Num_Last_RW+Flash_Service.Cnt_Bytes<4096)
                         {
                             SpiaRegs.SPITXBUF=0xAA00;
                             Flash_Service.Num_Last_RW++;
                         }
                     }
                     else
                     {
                         while(Flash_Service.Num_Last_RW<12)
                         {
                             SpiaRegs.SPITXBUF=0xAA00;
                             Flash_Service.Num_Last_RW++;
                         }
                     }
                 }
                 else
                 {
                     if(++Flash_Service.Cnt_Overtime>=Flash_Service.Ptr->Num_Overtime)
                     {
                         Flash_Service.Cnt_Overtime=0;
                         Flash_Service.Cnt_Bytes=0;
                         Flash_Service.Ptr->Result_RW=SPI_FLASH_WRITE_ERR;
                         Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                         Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                         Flash_Service.State=1;
                     }
                     else
                     {   //Flash���ڷǿ���״̬���ٴζ�ȡ״̬
                         Flash_Service.State=12;
                     }
                 }
             }
             else
             {
                 if(++Flash_Service.Cnt_Overtime>=Flash_Service.Ptr->Num_Overtime)
                 {
                     Flash_Service.Cnt_Overtime=0;
                     Flash_Service.Cnt_Bytes=0;
                     Flash_Service.Ptr->Result_RW=SPI_FLASH_WRITE_ERR;
                     Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                     Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                     Flash_Service.State=1;
                     SET_FLASH_CS;
                 }
                 else
                 {   //���յ����ֽ����������¶�ȡFLASH��״̬
                     Flash_Service.State=12;
                 }
             }
             break;}

  case 14:{SET_FLASH_CS;
          if(SpiaRegs.SPIFFRX.bit.RXFFST==Flash_Service.Num_Last_RW+4)
          {   //���յ��ֽ�����ȷ
              Uint16 Temp;
              Temp=SpiaRegs.SPIRXBUF;
              Temp=SpiaRegs.SPIRXBUF;
              Temp=SpiaRegs.SPIRXBUF;
              Temp=SpiaRegs.SPIRXBUF;

              Flash_Service.Cnt_Overtime=0;

              Flash_Service.Addr_RW_Now=Flash_Service.Addr_RW_Now+Flash_Service.Num_Last_RW;

              while((Flash_Service.Num_Last_RW>0)&&(Flash_Service.Cnt_Bytes<4096))
              {
             //     SectorImage[Flash_Service.Cnt_Bytes++]=SpiaRegs.SPIRXBUF&0x00FF;
                  Flash_Service.Num_Last_RW--;
              }

              if(Flash_Service.Cnt_Bytes>=4096)
              {
                  Flash_Service.State=15;
              }
              else
              {
                  Flash_Service.State=12;
              }
          }
          else
          {
              if(++Flash_Service.Cnt_Overtime>=Flash_Service.Ptr->Num_Overtime)
              {
                  Flash_Service.Cnt_Bytes=0;
                  Flash_Service.Cnt_Overtime=0;
                  Flash_Service.Ptr->Result_RW=SPI_FLASH_WRITE_ERR;
                  Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                  Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                  Flash_Service.State=1;
              }
              else
              {
                  Flash_Service.State=12;
              }
          }
          break;}

     case 15:{
             //�޸�RAM�е�Sector����
             Flash_Service.Cnt_Bytes=0;
             while(Flash_Service.Cnt_Bytes<Flash_Service.Ptr->Num_Bytes)
             {
          //       SectorImage[Flash_Service.Ptr->Offset_In_Sector+Flash_Service.Cnt_Bytes]=Flash_Service.Ptr->Data[Flash_Service.Cnt_Bytes];
                 Flash_Service.Cnt_Bytes++;
             }

             Flash_Service.Cnt_Overtime=0;
             Flash_Service.Addr_RW_Now=Flash_Service.Ptr->Offset_Sector<<12;
             //������ӦSector
             //CLEAR_FLASH_CS;
             //SpiaRegs.SPITXBUF=0x2000;
             //SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now>>8)&0x0000FF00;
             //SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now   )&0x0000FF00;
             //SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now<<8)&0x0000FF00;
             Flash_Service.State=16;
             break;}

     case 16:{SET_FLASH_CS;
             SpiaRegs.SPIFFRX.bit.RXFIFORESET=0;
             SpiaRegs.SPIFFRX.bit.RXFIFORESET=1;
             //������ӦSector
             CLEAR_FLASH_CS;
             SpiaRegs.SPITXBUF=0x0600;
             Flash_Service.State=17;
             break;}

     case 17:{SET_FLASH_CS;
             SpiaRegs.SPIFFRX.bit.RXFIFORESET=0;
             SpiaRegs.SPIFFRX.bit.RXFIFORESET=1;
             CLEAR_FLASH_CS;
             SpiaRegs.SPITXBUF=0x0500;
             SpiaRegs.SPITXBUF=0x0000;
             Flash_Service.State=18;
             break;}

     case 18:{SET_FLASH_CS;
             if(SpiaRegs.SPIFFRX.bit.RXFFST==2)
             {   //ȷ��дʹ��
                 Uint16 Temp;
                 Temp=SpiaRegs.SPIRXBUF;
                 Temp=SpiaRegs.SPIRXBUF;
                 if((Temp&0x02)==0x02)
                 {   //����Flash
                     CLEAR_FLASH_CS;
                     SpiaRegs.SPITXBUF=0x2000;
                     SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now>>8)&0x0000FF00;
                     SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now   )&0x0000FF00;
                     SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now<<8)&0x0000FF00;

                     Flash_Service.Cnt_Bytes=0;
                     Flash_Service.Cnt_Page_Write=0;
                     Flash_Service.Num_Last_RW=0;
                     Flash_Service.Addr_RW_Now=Flash_Service.Ptr->Offset_Sector<<12;
                     Flash_Service.Cnt_Overtime=0;

                     Flash_Service.State=19;
                 }
                 else
                 {
                     if(++Flash_Service.Cnt_Overtime>=4000)
                     {
                         Flash_Service.Cnt_Overtime=0;
                         Flash_Service.Cnt_Bytes=0;
                         Flash_Service.Ptr->Result_RW=SPI_FLASH_WRITE_ERR;
                         Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                         Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                         Flash_Service.State=1;
                     }
                     else
                     {
                         Flash_Service.State=16;
                     }
                 }
             }
             else
             {
                 if(++Flash_Service.Cnt_Overtime>=4000)
                 {
                     Flash_Service.Cnt_Overtime=0;
                     Flash_Service.Cnt_Bytes=0;
                     Flash_Service.Ptr->Result_RW=SPI_FLASH_WRITE_ERR;
                     Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                     Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                     Flash_Service.State=1;
                 }
                 else
                 {
                     Flash_Service.State=17;
                 }
             }
             break;}

     case 19:{SET_FLASH_CS;
             SpiaRegs.SPIFFRX.bit.RXFIFORESET=0;
             SpiaRegs.SPIFFRX.bit.RXFIFORESET=1;
             CLEAR_FLASH_CS;
             SpiaRegs.SPITXBUF=0x0500;
             SpiaRegs.SPITXBUF=0x0000;
             Flash_Service.State=20;
             break;}

     case 20:{SET_FLASH_CS;
             if(SpiaRegs.SPIFFRX.bit.RXFFST==2)
             {   //�ȴ�Sector�������
                 Uint16 Temp;
                 Temp=SpiaRegs.SPIRXBUF;
                 Temp=SpiaRegs.SPIRXBUF;
                 if((Temp&0x01)==0)
                 {   //Flashдʹ��
                     CLEAR_FLASH_CS;
                     Flash_Service.Cnt_Overtime=0;
                     Flash_Service.State=21;
                     SpiaRegs.SPITXBUF=0x0600;
                 }
                 else
                 {
                     if(++Flash_Service.Cnt_Overtime>=4000)
                     {
                         Flash_Service.Cnt_Overtime=0;
                         Flash_Service.Cnt_Bytes=0;
                         Flash_Service.Ptr->Result_RW=SPI_FLASH_WRITE_ERR;
                         Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                         Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                         Flash_Service.State=1;
                     }
                     else
                     {
                         Flash_Service.State=19;
                     }
                 }
             }
             else
             {
                 if(++Flash_Service.Cnt_Overtime>=4000)
                 {
                     Flash_Service.Cnt_Overtime=0;
                     Flash_Service.Cnt_Bytes=0;
                     Flash_Service.Ptr->Result_RW=SPI_FLASH_WRITE_ERR;
                     Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                     Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                     Flash_Service.State=1;
                 }
                 else
                 {
                     Flash_Service.State=19;
                 }
             }
             break;}

     case 21:{SET_FLASH_CS;
             SpiaRegs.SPIFFRX.bit.RXFIFORESET=0;
             SpiaRegs.SPIFFRX.bit.RXFIFORESET=1;
             CLEAR_FLASH_CS;
             SpiaRegs.SPITXBUF=0x0500;
             SpiaRegs.SPITXBUF=0x0000;
             Flash_Service.State=22;
             break;}

     case 22:{SET_FLASH_CS;
             if(SpiaRegs.SPIFFRX.bit.RXFFST==2)
             {   //�ȴ�Flash����
                 Uint16 Temp;
                 Temp=SpiaRegs.SPIRXBUF;
                 Temp=SpiaRegs.SPIRXBUF;
                 if((Temp&0x01)==0)
                 {   //��Flash����ҳ���
                     CLEAR_FLASH_CS;
                     Flash_Service.Cnt_Overtime=0;
                     SpiaRegs.SPITXBUF=0x0200;
                     SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now>>8)&0x0000FF00;
                     SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now   )&0x0000FF00;
                     SpiaRegs.SPITXBUF=(Flash_Service.Addr_RW_Now<<8)&0x0000FF00;
                     Flash_Service.State=23;
                 }
                 else
                 {
                     if(++Flash_Service.Cnt_Overtime>=1000)
                     {
                         Flash_Service.Cnt_Overtime=0;
                         Flash_Service.Cnt_Bytes=0;
                         Flash_Service.Ptr->Result_RW=SPI_FLASH_WRITE_ERR;
                         Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                         Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                         Flash_Service.State=1;
                     }
                     else
                     {
                         Flash_Service.State=21;
                     }
                 }
             }
             else
             {
                 if(++Flash_Service.Cnt_Overtime>=1000)
                 {
                     Flash_Service.Cnt_Overtime=0;
                     Flash_Service.Cnt_Bytes=0;
                     Flash_Service.Ptr->Result_RW=SPI_FLASH_WRITE_ERR;
                     Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                     Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                     Flash_Service.State=1;
                 }
                 else
                 {
                     Flash_Service.State=21;
                 }
             }
             break;}

     case 23:{
             while(SpiaRegs.SPIFFTX.bit.TXFFST>=3);
             Flash_Service.Num_Last_RW=0;
             while(Flash_Service.Num_Last_RW<16)
             {
         //        SpiaRegs.SPITXBUF=((Uint16)SectorImage[Flash_Service.Cnt_Bytes++])<<8;
                 Flash_Service.Num_Last_RW++;
             }
             Flash_Service.Addr_RW_Now=Flash_Service.Addr_RW_Now+16;
             if(++Flash_Service.Cnt_Page_Write>=16)
             {   //һҳ����д�����
                 Flash_Service.Cnt_Page_Write=0;
                 Flash_Service.State=19;
             }

             if(Flash_Service.Cnt_Bytes>=4096)
             {
                 Flash_Service.State=24;
             }

             break;}

     case 24:{SET_FLASH_CS;
             SpiaRegs.SPIFFRX.bit.RXFIFORESET=0;
             SpiaRegs.SPIFFRX.bit.RXFIFORESET=1;
             CLEAR_FLASH_CS;
             SpiaRegs.SPITXBUF=0x0500;
             SpiaRegs.SPITXBUF=0x0000;
             Flash_Service.State=25;
             break;}

     case 25:{SET_FLASH_CS;
             if(SpiaRegs.SPIFFRX.bit.RXFFST==2)
             {
                 Uint16 Temp;
                 Temp=SpiaRegs.SPIRXBUF;
                 Temp=SpiaRegs.SPIRXBUF;
                 if((Temp&0x01)==0)
                 {
                     CLEAR_FLASH_CS;
                     Flash_Service.Cnt_Overtime=0;
                     SpiaRegs.SPITXBUF=0x0400;
                     Flash_Service.State=26;
                 }
             }
             else
             {
                 if(++Flash_Service.Cnt_Overtime>=Flash_Service.Ptr->Num_Overtime)
                 {
                     Flash_Service.Cnt_Overtime=0;
                     Flash_Service.Cnt_Bytes=0;
                     Flash_Service.Ptr->Result_RW=SPI_FLASH_WRITE_ERR;
                     Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
                     Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
                     Flash_Service.State=1;
                 }
                 else
                 {
                     Flash_Service.State=24;
                 }
             }
             break;}

     case 26:{SET_FLASH_CS;
             Flash_Service.Cnt_Overtime=0;
             Flash_Service.Cnt_Bytes=0;
             Flash_Service.Ptr->Result_RW=SPI_FLASH_IDLE;//SPI_FLASH_WRITE_OK;
             Flash_Service.Ptr->Req_RW=REQ_SPI_FLASH_IDLE;
             Flash_Service.Ptr=Flash_Service.Ptr->Ptr_Next;
             Flash_Service.State=1;
             break;}
     default:{Flash_Service.State=1;}
    }
}

//File end
