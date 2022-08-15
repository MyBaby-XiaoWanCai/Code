/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: COM.c
// Created on: 2020-4-2
//     Author: LuoAnJiang
/**************************************************************************************/
#include "Headers_All.h"
//全局变量定义


//函数声明


//函数定义
//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void sACKCommand(void)
{
    SCIA_TX.Data[0] = 'A';
    SCIA_TX.Data[1] = 'C';
    SCIA_TX.Data[2] = 'K';
    SCIA_TX.Num_To_TX=3;
    SCIA_Start_TX();
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void sNAKCommand(void)
{
    SCIA_TX.Data[0] = 'N';
    SCIA_TX.Data[1] = 'A';
    SCIA_TX.Data[2] = 'K';
    SCIA_TX.Num_To_TX=3;
    SCIA_Start_TX();
}

//*********************************************************************************
//Description: M COMMAND
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void    sMCommand(void)
{
    unsigned char   ubCmdType1;
    unsigned char   ubCmdType2;
    Uint16  uwCabNum = 0;
    Uint16  uwCabNumH = 0;
    Uint16  uwCabNumL = 0;
    Uint16  uwModuleAdd = 0;
    Uint16  uwModuleAddH = 0;
    Uint16  uwModuleAddL = 0;
    ubCmdType1 = SCIA_RX.Data[2];
    ubCmdType2 = SCIA_RX.Data[5];
    if(ubCmdType1 == 'Q')
    {
        uwCabNum = Num_Cabinet;//sGetCabinetsNum();
        uwCabNumH = (unsigned char)(uwCabNum>>8);
        uwCabNumL = (unsigned char)uwCabNum;
        uwModuleAdd = Addr_Module;//sGetModuleAddress();
        uwModuleAddH = (unsigned char)(uwModuleAdd>>8);
        uwModuleAddL = (unsigned char)uwModuleAdd;
        SCIA_TX.Data[0] = 0x09;
        SCIA_TX.Data[1] = 'M';
        SCIA_TX.Data[2] = 'C';;
        SCIA_TX.Data[3] = uwCabNumH;
        SCIA_TX.Data[4] = uwCabNumL;
        SCIA_TX.Data[5] = 'A';
        SCIA_TX.Data[6] = uwModuleAddH;
        SCIA_TX.Data[7] = uwModuleAddL;
        SCIA_TX.Data[8] = 0x0a;
        SCIA_TX.Num_To_TX = 0x09;
        SCIA_Start_TX();
    }
    else if((ubCmdType1 == 'C')&&(ubCmdType2 == 'A'))
    {
        uwCabNumH = SCIA_RX.Data[3];
        uwCabNumL = SCIA_RX.Data[4];
        uwCabNum = (uwCabNumH<<8)+uwCabNumL;
        Num_Cabinet=uwCabNum;//sSetCabinetsNum(uwCabNum);
        EEPROM_Write_Uint16(cEepromCabinetsNum,Num_Cabinet);//debug sEepromWrite(cEepromCabinetsNum,sGetCabinetsNum(),2);DELAY_US(5000);
        uwModuleAddH = SCIA_RX.Data[6];
        uwModuleAddL = SCIA_RX.Data[7];
        uwModuleAdd = (uwModuleAddH<<8)+uwModuleAddL;
        Addr_Module=uwModuleAdd;//sSetModuleAddress(uwModuleAdd);
        EEPROM_Write_Uint16(cEepromModuleAddress,Addr_Module);//debug sEepromWrite(cEepromModuleAddress,sGetModuleAddress(),2);DELAY_US(5000);
        sACKCommand();
    }
    else
    {
        sNAKCommand();
    }
}

//*********************************************************************************
//Description: V COMMAND
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void    sVCommand(void)
{
    SCIA_TX.Data[0] = 0x07;
    SCIA_TX.Data[1] = ubVerbNum[0];
    SCIA_TX.Data[2] = ubVerbNum[1];
    SCIA_TX.Data[3] = ubVerbNum[2];
    SCIA_TX.Data[4] = ubVerbNum[3];
    SCIA_TX.Data[5] = ubVerbNum[4];
    SCIA_TX.Data[6] = 0x0A;
    SCIA_TX.Num_To_TX=7;
    SCIA_Start_TX();
}

//函数定义
//*********************************************************************************
//Description: T COMMAND
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void sTCommand(void)
{
    unsigned char   ubCmdType1 = 0;
    ubCmdType1 = SCIA_RX.Data[2];
    if(ubCmdType1=='Q')
    {
        SCIA_TX.Data[0] = 0x1C;
        SCIA_TX.Data[1] = 'T';
        //GATEWAY
        SCIA_TX.Data[2] = 'G';
        SCIA_TX.Data[3] = W5500_Service.GAR[0];
        SCIA_TX.Data[4] = W5500_Service.GAR[1];
        SCIA_TX.Data[5] = W5500_Service.GAR[2];
        SCIA_TX.Data[6] = W5500_Service.GAR[3];
        //SUBNET MASK
        SCIA_TX.Data[7] = 'S';
        SCIA_TX.Data[8] = W5500_Service.SUBR[0];
        SCIA_TX.Data[9] = W5500_Service.SUBR[1];
        SCIA_TX.Data[10] = W5500_Service.SUBR[2];
        SCIA_TX.Data[11] = W5500_Service.SUBR[3];
        //MAC ADDRESS
        SCIA_TX.Data[12] = 'M';
        SCIA_TX.Data[13] = W5500_Service.SHAR[0];
        SCIA_TX.Data[14] = W5500_Service.SHAR[1];
        SCIA_TX.Data[15] = W5500_Service.SHAR[2];
        SCIA_TX.Data[16] = W5500_Service.SHAR[3];
        SCIA_TX.Data[17] = W5500_Service.SHAR[4];
        SCIA_TX.Data[18] = W5500_Service.SHAR[5];
        //IP ADDRESS
        SCIA_TX.Data[19] = 'I';
        SCIA_TX.Data[20] = W5500_Service.SIPR[0];
        SCIA_TX.Data[21] = W5500_Service.SIPR[1];
        SCIA_TX.Data[22] = W5500_Service.SIPR[2];
        SCIA_TX.Data[23] = W5500_Service.SIPR[3];
        //PORT NUM
        SCIA_TX.Data[24] = 'P';
        SCIA_TX.Data[25] = (TCP_Socket[SERV_TCP_WORK].Port_Source>>8)&0x00FF;
        SCIA_TX.Data[26] = (TCP_Socket[SERV_TCP_WORK].Port_Source)&0x00FF;
        //END
        SCIA_TX.Data[27] = 0x0a;
        SCIA_TX.Num_To_TX=28;
        SCIA_Start_TX();
    }
    else if(ubCmdType1=='G')
    {
        //网关数据
        W5500_Service.GAR[0] = SCIA_RX.Data[3];
        W5500_Service.GAR[1] = SCIA_RX.Data[4];
        W5500_Service.GAR[2] = SCIA_RX.Data[5];
        W5500_Service.GAR[3] = SCIA_RX.Data[6];

        EEPROM_Write_Uint16(cEepromGateway_IP0,W5500_Service.GAR[0]);
        EEPROM_Write_Uint16(cEepromGateway_IP1,W5500_Service.GAR[1]);
        EEPROM_Write_Uint16(cEepromGateway_IP2,W5500_Service.GAR[2]);
        EEPROM_Write_Uint16(cEepromGateway_IP3,W5500_Service.GAR[3]);
        sACKCommand();
    }
    else if(ubCmdType1=='S')
    {
        //子网掩码
        W5500_Service.SUBR[0] = SCIA_RX.Data[3];
        W5500_Service.SUBR[1] = SCIA_RX.Data[4];
        W5500_Service.SUBR[2] = SCIA_RX.Data[5];
        W5500_Service.SUBR[3] = SCIA_RX.Data[6];

        EEPROM_Write_Uint16(cEepromSub_Mask0,W5500_Service.SUBR[0]);
        EEPROM_Write_Uint16(cEepromSub_Mask1,W5500_Service.SUBR[1]);
        EEPROM_Write_Uint16(cEepromSub_Mask2,W5500_Service.SUBR[2]);
        EEPROM_Write_Uint16(cEepromSub_Mask3,W5500_Service.SUBR[3]);

        sACKCommand();
    }
    else if(ubCmdType1=='M')
    {
        //物理地址
        W5500_Service.SHAR[0] = SCIA_RX.Data[3];
        W5500_Service.SHAR[1] = SCIA_RX.Data[4];
        W5500_Service.SHAR[2] = SCIA_RX.Data[5];
        W5500_Service.SHAR[3] = SCIA_RX.Data[6];
        W5500_Service.SHAR[4] = SCIA_RX.Data[7];
        W5500_Service.SHAR[5] = SCIA_RX.Data[8];

        EEPROM_Write_Uint16(cEepromPhy_Addr0,W5500_Service.SHAR[0]);
        EEPROM_Write_Uint16(cEepromPhy_Addr1,W5500_Service.SHAR[1]);
        EEPROM_Write_Uint16(cEepromPhy_Addr2,W5500_Service.SHAR[2]);
        EEPROM_Write_Uint16(cEepromPhy_Addr3,W5500_Service.SHAR[3]);
        EEPROM_Write_Uint16(cEepromPhy_Addr4,W5500_Service.SHAR[4]);
        EEPROM_Write_Uint16(cEepromPhy_Addr5,W5500_Service.SHAR[5]);
        sACKCommand();
    }
    else if(ubCmdType1=='I')
    {
        //IP地址
        W5500_Service.SIPR[0] = SCIA_RX.Data[3];
        W5500_Service.SIPR[1] = SCIA_RX.Data[4];
        W5500_Service.SIPR[2] = SCIA_RX.Data[5];
        W5500_Service.SIPR[3] = SCIA_RX.Data[6];

        EEPROM_Write_Uint16(cEepromIP_Addr0,W5500_Service.SIPR[0]);
        EEPROM_Write_Uint16(cEepromIP_Addr1,W5500_Service.SIPR[1]);
        EEPROM_Write_Uint16(cEepromIP_Addr2,W5500_Service.SIPR[2]);
        EEPROM_Write_Uint16(cEepromIP_Addr3,W5500_Service.SIPR[3]);

        sACKCommand();

        if((W5500_Service.SIPR[3]==100)||(W5500_Service.SIPR[3]==102)||(W5500_Service.SIPR[3]==104)||(W5500_Service.SIPR[3]==106)||(W5500_Service.SIPR[3]==108))
        {
            Auto_Aging.Req_WR_M1=TRUE;
        }

        if((W5500_Service.SIPR[3]==101)||(W5500_Service.SIPR[3]==103)||(W5500_Service.SIPR[3]==105)||(W5500_Service.SIPR[3]==107)||(W5500_Service.SIPR[3]==109))
        {
            Auto_Aging.Req_WR_M2=TRUE;
        }
    }
    else if(ubCmdType1=='P')
    {
        //端口地址
        TCP_Socket[SERV_TCP_WORK].Port_Source = ((Uint16)SCIA_RX.Data[3]<<8)+SCIA_RX.Data[4];
        EEPROM_Write_Uint16(cEepromS0_Port0,(TCP_Socket[SERV_TCP_WORK].Port_Source>>8)&0x00FF);
        EEPROM_Write_Uint16(cEepromS0_Port1,TCP_Socket[SERV_TCP_WORK].Port_Source&0x00FF);
        sACKCommand();
    }
    else
    {
        sNAKCommand();
    }
}

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void sNullCommand(void)
{
    sNAKCommand();
}

typedef void (*ptrFun)();
static const  ptrFun sSciFunJump[] =
{
        sNullCommand,//sACommand,          //A     //校准用默认值写入,去掉
        sNullCommand,       //B
        sNullCommand,//sCCommand,          //C     //状态控制，去掉
        sNullCommand,       //D
        sNullCommand,       //E
        sNullCommand,       //F
        sNullCommand,       //G
        sNullCommand,       //H
        sNullCommand,       //I
        sNullCommand,       //J
        sNullCommand,       //K
        sNullCommand,       //L
        sMCommand,          //M     //库位号与通道号查询，保留
        sNullCommand,       //N
        sNullCommand,       //O
        sNullCommand,//sPCommand,   //P PI参数相关设置去掉
        sNullCommand,//sQCommand,          //Q     //查询相关，去掉
        sNullCommand,       //R
        sNullCommand,//sSCommand,          //S     //空函数，去掉
        sTCommand,          //T     //IP地址相关设置，保留
        sNullCommand,       //U
        sVCommand,          //V     //固件版本号读取，保留
        sNullCommand,       //W
        sNullCommand,       //X
        sNullCommand,       //Y
        sNullCommand,//sZCommand,          //Z     //调试相关，去掉
};

//*********************************************************************************
//Description:
//Parameters : NONE
//Return     : NONE
//*********************************************************************************
void sSCIAParsing(void)
{
    unsigned char ubCmdTemp;

    if((SCIA_RX.State_Recived==TRUE)&&(SCIA_TX.State_TX==FALSE))
    {
        ubCmdTemp = SCIA_RX.Data[1];

        if(ubCmdTemp >= 'A' && ubCmdTemp <= 'Z')
        {
            sSciFunJump[ubCmdTemp - 'A']();
        }
        else
        {
            sNAKCommand();
        }
        SCIA_RX.Cnt_RX=0;
        SCIA_RX.Cnt_Timeout=0;
        SCIA_RX.State_Recived=FALSE;
    }
}

//File end
