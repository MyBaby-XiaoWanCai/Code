/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_MB85RS2.c
** Author      : wang liang
** Date        : 2021-11-09
** Description : bsp_MB85RS2 source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
__IO uint8_t Fram_Op = 0;            //���������־ 1�����ڲ����� 0:û�в���
uint32_t MB85RS2_Size;               //�����С����ʼ�����ȡ��Ч
static UserFunMB85_Type User_Reg;    //�û�ע�ắ���ṹ��
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
static uint8_t SPI_ReadWriteByte(uint8_t data,uint8_t index);

/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
                                          Ĭ�Ͻӿ�ע�ắ��
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/**********************************************************************************************************
**	�� �� �� : MB85_delyus()
**	����˵�� : Ĭ��US��ʱ������ʹ������ǰ���û����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void MB85_delyus(uint32_t us)
{
    bsp_DelayUS(us);
}
/**********************************************************************************************************
**	�� �� �� : MB85_delyms()
**	����˵�� : Ĭ��MS��ʱ������ʹ������ǰ���û����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void MB85_delyms(uint32_t ms)
{
    bsp_DelayMS(ms);
}


/**********************************************************************************************************
**	�� �� �� : MB85_cs0_select()
**	����˵�� : Ĭ��Ƭѡ����0��ʹ������ǰ���û����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void MB85_cs0_select()
{
    GPIO_ResetPins(MB85R_NSS_PORT, MB85R_NSS_PIN);
}


/**********************************************************************************************************
**	�� �� �� : MB85_cs0_deselect()
**	����˵�� : Ĭ�Ͻ��Ƭѡ����0��ʹ������ǰ���û����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void MB85_cs0_diselect()
{
    GPIO_SetPins(MB85R_NSS_PORT, MB85R_NSS_PIN);
}


/**********************************************************************************************************
**	�� �� �� : MB85_cris_enter()
**	����˵�� : Ĭ�Ͻ����ٽ纯����ʹ������ǰ���û����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void MB85_cris_enter()
{

}

/**********************************************************************************************************
**	�� �� �� : MB85_cris_exit()
**	����˵�� : Ĭ���˳��ٽ纯����ʹ������ǰ���û����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void MB85_cris_exit()
{

}
/**********************************************************************************************************
**	�� �� �� : MB85_spi0_tranRebyte()
**	����˵�� : Ĭ�ϵ��ֽ�д�ض�������ʹ������ǰ���û����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
uint32_t MB85_spi0_tranRebyte(const void  * wb,void *rb,uint32_t len)
{
    return SPI_TransmitReceive(MB85R_SPI_UNIT, wb, rb, len);
}


/**********************************************************************************************************
**	�� �� �� : MB85_port0_init()
**	����˵�� : Ĭ�Ͻӿ�IO��ʼ��������ʹ������ǰ���û����
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void MB85_port0_init()
{

    stc_gpio_init_t stcGpioCfg;
    stc_spi_init_t stcSpiInit;

    /* Configure peripheral clock */
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_SPI6, Enable);

    /* Port configurate, High driving capacity for output pin. */
    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDir = PIN_DIR_OUT;
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    stcGpioCfg.u16PinState = PIN_STATE_SET;
    (void)GPIO_Init(MB85R_NSS_PORT,  MB85R_NSS_PIN, &stcGpioCfg);           //��ʼ��Ƭѡ�źŽ�

    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(MB85R_SCK_PORT,  MB85R_SCK_PIN, &stcGpioCfg);
    (void)GPIO_Init(MB85R_MOSI_PORT, MB85R_MOSI_PIN, &stcGpioCfg);           //��ʼ��SPI���

    /* CMOS input for input pin */
    stcGpioCfg.u16PinDrv = PIN_DRV_LOW;
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(MB85R_MISO_PORT, MB85R_MISO_PIN, &stcGpioCfg);           //��ʼ��SPI����

    /* Configure SPI Port function for master */
    GPIO_SetFunc(MB85R_SCK_PORT,  MB85R_SCK_PIN,  W25Q_SCK_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(MB85R_MISO_PORT, MB85R_MISO_PIN, W25Q_MISO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(MB85R_MOSI_PORT, MB85R_MOSI_PIN, W25Q_MOSI_FUNC, PIN_SUBFUNC_DISABLE);

    /* SPI De-initialize */
    SPI_DeInit(MB85R_SPI_UNIT);
    /* Clear initialize structure */
    (void)SPI_StructInit(&stcSpiInit);

    stcSpiInit.u32WireMode          = SPI_WIRE_3;
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave       = SPI_MASTER;
    stcSpiInit.u32SuspMode          = SPI_COM_SUSP_FUNC_OFF;
    stcSpiInit.u32Modfe             = SPI_MODFE_DISABLE;
    stcSpiInit.u32Parity            = SPI_PARITY_INVALID;
    stcSpiInit.u32SpiMode           = SPI_MODE_3;
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_PCLK1_DIV8;                    //������=120M/8=15M
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;
    (void)SPI_Init(MB85R_SPI_UNIT, &stcSpiInit);
    GPIO_SetPins(MB85R_NSS_PORT, MB85R_NSS_PIN);

    /* Command SPI function */
    SPI_FunctionCmd(MB85R_SPI_UNIT, Enable);
  
}


/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
                                          �����ӿ�ע�ắ��
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

/**********************************************************************************************************
**	�� �� �� : Reg_User_DELAY_Func()
**	����˵�� : ע���û�Ƭ��ʱ����
**	��    �� : void(*res0_en)(void)���û���λ����
**	�� �� ֵ : ��
**********************************************************************************************************/
static void Reg_User_DELAY_Func(void (*_delayus)(uint32_t us), void (*_delayms)(uint32_t ms))
{
    //����û�ע�ắ��ΪNALL��ʹ��Ĭ�ϸ�λ����������ʹ���û�ָ��Ƭ��λ����
    if (!_delayus)
    {
        User_Reg.DELAY_MB85._delyus = MB85_delyus;
    }
    else
    {
        User_Reg.DELAY_MB85._delyus = _delayus;
    }

    if (!_delayms)
    {
        User_Reg.DELAY_MB85._delyms = MB85_delyms;
    }
    else
    {
        User_Reg.DELAY_MB85._delyms = _delayms;
    }
}


/**********************************************************************************************************
**	�� �� �� : Reg_User_CS_Func()
**	����˵�� : ע���û�Ƭѡ�ͽ��Ƭѡ����
**	��    �� : void(*cs_sel)(void)���û�Ƭѡ������void(*cs_desel)(void)���û����Ƭѡ������ index��ע�ᵽ�ڼ�Ƭ
**	�� �� ֵ : ��
**********************************************************************************************************/
static void Reg_User_CS_Func(void (*cs0_sel)(void), void (*cs0_desel)(void), uint8_t index)
{
    if(index>=MX_NUM)  //���ע��Ƭ�ű���С��MX_NUM��Ƭ����Ч��Χ��0~MX_NUM-1
        return;
    //����û�ע�ắ��ΪNALL��ʹ��Ĭ��Ƭѡ�ͽ��Ƭѡ����������ʹ���û�ָ��Ƭѡ�ͽ��Ƭѡ����
    if (!cs0_sel || !cs0_desel)
    {
        User_Reg.CS_MB85[index]._select=MB85_cs0_select;
        User_Reg.CS_MB85[index]._diselect=MB85_cs0_diselect;
    }
    else
    {
        User_Reg.CS_MB85[index]._select = cs0_sel;
        User_Reg.CS_MB85[index]._diselect = cs0_desel;
    }
}

/**********************************************************************************************************
**	�� �� �� : Reg_User_CRIS_Func()
**	����˵�� : ע���û������ٽ�������
**	��    �� : void(*cris_en)(void)���û����ٽ���������void(*cris_ex)(void)���û����ٽ�������
**	�� �� ֵ : ��
**********************************************************************************************************/
static void Reg_User_CRIS_Func(void (*cris_en)(void), void (*cris_ex)(void))
{
    //����û�ע�ắ��ΪNALL��ʹ��Ĭ�Ͻ�����˳��ٽ纯��������ʹ���û�ָ��������˳��ٽ纯��
    if (!cris_en || !cris_ex)
    {
        User_Reg.CRIS0_MB85._enter = MB85_cris_enter;
        User_Reg.CRIS0_MB85._exit = MB85_cris_exit;
    }
    else
    {
        User_Reg.CRIS0_MB85._enter = cris_en;
        User_Reg.CRIS0_MB85._exit = cris_ex;
    }
}

/**********************************************************************************************************
**	�� �� �� : Reg_User_SPI_Func()
**	����˵�� : ע���û�SPI���ͳ�ʼ������
**	��    �� : (uint32_t (*spi_wrb)(const void * wb, void *rb,uint32_t len)���û�SPI���ֽ�д�ض�����,
               void (*port_init)(void)���û��ӿڳ�ʼ������
**	�� �� ֵ : ��
**********************************************************************************************************/
static void Reg_User_SPI_Func( uint32_t (*spi0_wrb)(const void * wb, void *rb,uint32_t len),\
                               void (*port0_init)(void), uint8_t index)
{

    if(index>=MX_NUM)  //ע��Ƭ��������С��MX_NUM��Ƭ����Ч��Χ��0~MX_NUM-1
        return;
    //����û�ע�ắ��ΪNALL��ʹ��Ĭ��SPI�������ͽӿڳ�ʼ������������ʹ���û�ָ��SPI�������ͽӿڳ�ʼ������
    if (!spi0_wrb)
    {
        User_Reg.SPI_MB85[index]._tran_re_byte = MB85_spi0_tranRebyte;
    }
    else
    {
        User_Reg.SPI_MB85[index]._tran_re_byte = spi0_wrb;
    }


    if (!port0_init)
    {
        User_Reg.SPI_MB85[index]._init = MB85_port0_init;
    }
    else
    {
        User_Reg.SPI_MB85[index]._init = port0_init;
    }
}


/**********************************************************************************************************
**	�� �� �� : Reg_User_Init()
**	����˵�� : ע���û���ʼ������
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
static void Reg_User_Init(void)
{
    Reg_User_DELAY_Func(NULL, NULL);                 //ʹ��Ĭ����ʱ����
    Reg_User_CS_Func(NULL, NULL,0);                  //ע��Ƭ��0��CS��ʹ��Ĭ��Ƭѡ�����Ƭѡ����
    Reg_User_CRIS_Func(NULL, NULL);                  //ʹ��Ĭ���ٽ��������
    Reg_User_SPI_Func(NULL, NULL,0);                 //ע��Ƭ��0��SPI�ͳ�ʼ����ʹ��Ĭ��SPI���������ӿڳ�ʼ������
}

/*
*********************************************************************************************************
*   �� �� ��: bsp_InitMB85R
*   ����˵��: �����ʼ���������Զ�ʶ���ͺţ��洢��С
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitMB85R(void)
{
    Reg_User_Init();                     //ע�������ӿ�
    User_Reg.SPI_MB85[0]._init();        //��ʼ��IO
    MB85Ctrl_ReadID(&User_Reg.ID[0],0);  //��ȡID
    if((User_Reg.ID[0] & 0x00000F00)==0x00000800)              //2M�ͺ�,24λ��ַ
    {
        User_Reg.MB85RS2_Size[0]=0x40000;                      //256K bytes
    } else if ((User_Reg.ID[0] & 0x00000F00)==0x00000100)      //16K�ͺţ�16λ��ַ
    {
        User_Reg.MB85RS2_Size[0]=0x4000;                       //2K bytes
    }

    MB85RS2_Size=User_Reg.MB85RS2_Size[0];

}


/*
*********************************************************************************************************
*   �� �� ��: SPI_ReadWriteByte
*   ����˵��: SPI�ֽڶ�д
*   ��    ��: data��������������, index: ָ��Ƭ��
*   �� �� ֵ: �ӻ���������
*********************************************************************************************************
*/
static uint8_t SPI_ReadWriteByte(uint8_t data,uint8_t index)
{
    uint8_t Rx_data = 0;
    User_Reg.SPI_MB85[index]._tran_re_byte(&data, &Rx_data, 1);
    return Rx_data;
}

/*
*********************************************************************************************************
*   �� �� ��: MB85Ctrl_Write
*   ����˵��: ��FRAMָ����ַд����
*   ��    ��: addr��Ҫд��ĵ�ַ��buf��Ҫд�������ָ�룬len�����ݳ����ֽ� index: ָ��Ƭ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void MB85Ctrl_Write(uint32_t addr, uint8_t *buf, uint16_t len, uint8_t index)
{
    uint8_t addr_H, addr_M, addr_L;

    addr_H = (uint8_t)((addr & 0xff0000) >> 16);
    addr_M = (uint8_t)((addr & 0xff00) >> 8);
    addr_L = (uint8_t)(addr & 0x00ff);

    User_Reg.CS_MB85[index]._select();             //Ƭѡѡ��
    SPI_ReadWriteByte(MB85RS2_CMD_WREN, index);    //���ͽ��д����ָ��
    User_Reg.CS_MB85[index]._diselect();           //���Ƭѡ

    User_Reg.DELAY_MB85._delyus(1);

    User_Reg.CS_MB85[index]._select();                          //Ƭѡѡ��
    SPI_ReadWriteByte(MB85RS2_CMD_WRITE,index);                 //����д����ָ��
    if((User_Reg.ID[index] & 0x00000F00)==0x00000800)           //2M�ͺ�,24λ��ַ
    {
        SPI_ReadWriteByte(addr_H,index);                            //����д��ַ
        SPI_ReadWriteByte(addr_M,index);
        SPI_ReadWriteByte(addr_L,index);
    } else if ((User_Reg.ID[index] & 0x00000F00)==0x00000100)    //16K�ͺţ�16λ��ַ
    {
        SPI_ReadWriteByte(addr_M,index);                            //����д��ַ
        SPI_ReadWriteByte(addr_L,index);
    }

    for(; len > 0; len--)
    {
        SPI_ReadWriteByte(*buf++,index);           //����д����
    }
    User_Reg.CS_MB85[index]._diselect();           //���Ƭѡ
}

/*
*********************************************************************************************************
*   �� �� ��: MB85Ctrl_Read
*   ����˵��: ����FRAMָ����ַ����
*   ��    ��: addr��Ҫ�����ĵ�ַ��buf�����������ݴ洢ָ�룬len�����ݳ����ֽ� index: ָ��Ƭ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void MB85Ctrl_Read(uint32_t addr, uint8_t *buf, uint16_t len, uint8_t index)
{
    uint8_t addr_H, addr_M, addr_L;

    addr_H = (uint8_t)((addr & 0xff0000) >> 16);
    addr_M = (uint8_t)((addr & 0xff00) >> 8);
    addr_L = (uint8_t)(addr & 0x00ff);

    User_Reg.CS_MB85[index]._select();                          //Ƭѡѡ��
    SPI_ReadWriteByte(MB85RS2_CMD_READ,index);                  //���Ͷ�����ָ��
    if((User_Reg.ID[index] & 0x00000F00)==0x00000800)           //2M�ͺ�,24λ��ַ
    {
        SPI_ReadWriteByte(addr_H,index);                        //���Ͷ���ַ
        SPI_ReadWriteByte(addr_M,index);
        SPI_ReadWriteByte(addr_L,index);
    } else if ((User_Reg.ID[index] & 0x00000F00)==0x00000100)    //16K�ͺţ�16λ��ַ
    {
        SPI_ReadWriteByte(addr_M,index);                         //���Ͷ���ַ
        SPI_ReadWriteByte(addr_L,index);
    }
    for(; len > 0; len--)
    {
        *buf++ = SPI_ReadWriteByte(0x00, index); //��ָ�����ȶ�����
    }
    User_Reg.CS_MB85[index]._diselect();         //���Ƭѡ
}

/*
*********************************************************************************************************
*   �� �� ��: MB85Ctrl_ReadID
*   ����˵��: ����FRAM����32λ����ID
*   ��    ��: buf�����������ݴ洢ָ�� index: ָ��Ƭ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void MB85Ctrl_ReadID(uint32_t *buf,uint8_t index)
{
    uint8_t i;
    uint8_t bytebuf[4]= {0};

    User_Reg.CS_MB85[index]._select();              //Ƭѡѡ��
    SPI_ReadWriteByte(MB85RS2_CMD_RDID, index);     //���Ͷ�ID����ָ��
    for(i=0; i < 4; i++)
    {
        bytebuf[i] = SPI_ReadWriteByte(0x00,index); //��ȡIDֵ
    }
    *buf=(bytebuf[0]<<24)+(bytebuf[1]<<16)+(bytebuf[2]<<8)+(bytebuf[3]);  //��С������
    User_Reg.CS_MB85[index]._diselect();            //���Ƭѡ
}

/*
*********************************************************************************************************
*   �� �� ��: MB85Ctrl_CheckWrite
*   ����˵��: ��FRAMָ����ַд����,���������ȷ��д���Ƿ���ȷ
*   ��    ��: addr��Ҫд��ĵ�ַ��buf��Ҫд�������ָ�룬len�����ݳ����ֽ� index: ָ��Ƭ��
*   �� �� ֵ: 1��д��ɹ���0��д��ʧ��
*********************************************************************************************************
*/
uint8_t MB85Ctrl_CheckWrite(uint32_t addr, uint8_t *buf, uint16_t len, uint8_t index)
{
 #if USE_OS
    int32_t ls, us;
    Fram_Op=1;
    Dis_int_UserDefine();
    ls = osKernelLock();
 #endif   
#if 0     /*ʹ��malloc,free���������´���*/
    uint8_t *date = (uint8_t *)malloc(len);
    uint8_t i;
    uint8_t eq = 1;
    MB85Ctrl_Write(addr, buf, len);
    MB85Ctrl_Read(addr, date, len);

    for(i = 0; i < len; i++)
    {
        if(date[i] != buf[i])
        {
            eq = 0;
            break;
        }
    }
    free(date);
    return eq;
#elif 1
    /*��ʹ��malloc,free���������´���*/
    uint8_t date;
    uint16_t i;
    uint8_t eq = 1;
    MB85Ctrl_Write(addr, buf, len, index);
    for(i = 0; i < len; i++)
    {
        MB85Ctrl_Read(addr + i, &date, 1, index);
        if(date != buf[i])
        {
            eq = 0;
#if ALRM_MB85_EN
            for(uint8_t j = 0; j < CHANNEL_CFG; j++)
            {
                IP_Alarm_Flag[j] |= IntelProtect_FRE;
            } //fram����
#endif
            break;
        }
    }
#if USE_OS
    us = osKernelUnlock();
    osKernelRestoreLock(us);
    osKernelRestoreLock(ls);
    osDelay(1);
    Fram_Op=0;
    En_int_UserDefine();
#endif
    return eq;
#endif
}
