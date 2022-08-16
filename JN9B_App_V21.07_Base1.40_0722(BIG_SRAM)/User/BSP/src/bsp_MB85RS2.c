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
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/
__IO uint8_t Fram_Op = 0;            //铁电操作标志 1：正在操作， 0:没有操作
uint32_t MB85RS2_Size;               //铁电大小，初始化后读取有效
static UserFunMB85_Type User_Reg;    //用户注册函数结构体
/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static uint8_t SPI_ReadWriteByte(uint8_t data,uint8_t index);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
                                          默认接口注册函数
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/**********************************************************************************************************
**	函 数 名 : MB85_delyus()
**	功能说明 : 默认US延时函数，使用驱动前由用户填充
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void MB85_delyus(uint32_t us)
{
    bsp_DelayUS(us);
}
/**********************************************************************************************************
**	函 数 名 : MB85_delyms()
**	功能说明 : 默认MS延时函数，使用驱动前由用户填充
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void MB85_delyms(uint32_t ms)
{
    bsp_DelayMS(ms);
}


/**********************************************************************************************************
**	函 数 名 : MB85_cs0_select()
**	功能说明 : 默认片选函数0，使用驱动前由用户填充
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void MB85_cs0_select()
{
    GPIO_ResetPins(MB85R_NSS_PORT, MB85R_NSS_PIN);
}


/**********************************************************************************************************
**	函 数 名 : MB85_cs0_deselect()
**	功能说明 : 默认解除片选函数0，使用驱动前由用户填充
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void MB85_cs0_diselect()
{
    GPIO_SetPins(MB85R_NSS_PORT, MB85R_NSS_PIN);
}


/**********************************************************************************************************
**	函 数 名 : MB85_cris_enter()
**	功能说明 : 默认进入临界函数，使用驱动前由用户填充
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void MB85_cris_enter()
{

}

/**********************************************************************************************************
**	函 数 名 : MB85_cris_exit()
**	功能说明 : 默认退出临界函数，使用驱动前由用户填充
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
void MB85_cris_exit()
{

}
/**********************************************************************************************************
**	函 数 名 : MB85_spi0_tranRebyte()
**	功能说明 : 默认单字节写回读函数，使用驱动前由用户填充
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
uint32_t MB85_spi0_tranRebyte(const void  * wb,void *rb,uint32_t len)
{
    return SPI_TransmitReceive(MB85R_SPI_UNIT, wb, rb, len);
}


/**********************************************************************************************************
**	函 数 名 : MB85_port0_init()
**	功能说明 : 默认接口IO初始化函数，使用驱动前由用户填充
**	形    参 : 无
**	返 回 值 : 无
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
    (void)GPIO_Init(MB85R_NSS_PORT,  MB85R_NSS_PIN, &stcGpioCfg);           //初始化片选信号脚

    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(MB85R_SCK_PORT,  MB85R_SCK_PIN, &stcGpioCfg);
    (void)GPIO_Init(MB85R_MOSI_PORT, MB85R_MOSI_PIN, &stcGpioCfg);           //初始化SPI输出

    /* CMOS input for input pin */
    stcGpioCfg.u16PinDrv = PIN_DRV_LOW;
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(MB85R_MISO_PORT, MB85R_MISO_PIN, &stcGpioCfg);           //初始化SPI输入

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
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_PCLK1_DIV8;                    //波特率=120M/8=15M
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;
    (void)SPI_Init(MB85R_SPI_UNIT, &stcSpiInit);
    GPIO_SetPins(MB85R_NSS_PORT, MB85R_NSS_PIN);

    /* Command SPI function */
    SPI_FunctionCmd(MB85R_SPI_UNIT, Enable);
  
}


/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
                                          驱动接口注册函数
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

/**********************************************************************************************************
**	函 数 名 : Reg_User_DELAY_Func()
**	功能说明 : 注册用户片延时函数
**	形    参 : void(*res0_en)(void)：用户复位函数
**	返 回 值 : 无
**********************************************************************************************************/
static void Reg_User_DELAY_Func(void (*_delayus)(uint32_t us), void (*_delayms)(uint32_t ms))
{
    //如果用户注册函数为NALL，使用默认复位函数，否则使用用户指定片复位函数
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
**	函 数 名 : Reg_User_CS_Func()
**	功能说明 : 注册用户片选和解除片选函数
**	形    参 : void(*cs_sel)(void)：用户片选函数，void(*cs_desel)(void)：用户解除片选函数， index：注册到第几片
**	返 回 值 : 无
**********************************************************************************************************/
static void Reg_User_CS_Func(void (*cs0_sel)(void), void (*cs0_desel)(void), uint8_t index)
{
    if(index>=MX_NUM)  //最大注册片号必须小于MX_NUM，片号有效范围：0~MX_NUM-1
        return;
    //如果用户注册函数为NALL，使用默认片选和解除片选函数，否则使用用户指定片选和解除片选函数
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
**	函 数 名 : Reg_User_CRIS_Func()
**	功能说明 : 注册用户进出临界区函数
**	形    参 : void(*cris_en)(void)：用户进临界区函数，void(*cris_ex)(void)：用户出临界区函数
**	返 回 值 : 无
**********************************************************************************************************/
static void Reg_User_CRIS_Func(void (*cris_en)(void), void (*cris_ex)(void))
{
    //如果用户注册函数为NALL，使用默认进入和退出临界函数，否则使用用户指定进入和退出临界函数
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
**	函 数 名 : Reg_User_SPI_Func()
**	功能说明 : 注册用户SPI读和初始化函数
**	形    参 : (uint32_t (*spi_wrb)(const void * wb, void *rb,uint32_t len)：用户SPI单字节写回读函数,
               void (*port_init)(void)：用户接口初始化函数
**	返 回 值 : 无
**********************************************************************************************************/
static void Reg_User_SPI_Func( uint32_t (*spi0_wrb)(const void * wb, void *rb,uint32_t len),\
                               void (*port0_init)(void), uint8_t index)
{

    if(index>=MX_NUM)  //注册片号最大必须小于MX_NUM，片号有效范围：0~MX_NUM-1
        return;
    //如果用户注册函数为NALL，使用默认SPI读函数和接口初始化函数，否则使用用户指定SPI读函数和接口初始化函数
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
**	函 数 名 : Reg_User_Init()
**	功能说明 : 注册用户初始化函数
**	形    参 : 无
**	返 回 值 : 无
**********************************************************************************************************/
static void Reg_User_Init(void)
{
    Reg_User_DELAY_Func(NULL, NULL);                 //使用默认延时函数
    Reg_User_CS_Func(NULL, NULL,0);                  //注册片号0的CS，使用默认片选，解除片选函数
    Reg_User_CRIS_Func(NULL, NULL);                  //使用默认临界进出函数
    Reg_User_SPI_Func(NULL, NULL,0);                 //注册片号0的SPI和初始化，使用默认SPI读函数，接口初始化函数
}

/*
*********************************************************************************************************
*   函 数 名: bsp_InitMB85R
*   功能说明: 铁电初始化函数，自动识别型号，存储大小
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitMB85R(void)
{
    Reg_User_Init();                     //注册驱动接口
    User_Reg.SPI_MB85[0]._init();        //初始化IO
    MB85Ctrl_ReadID(&User_Reg.ID[0],0);  //读取ID
    if((User_Reg.ID[0] & 0x00000F00)==0x00000800)              //2M型号,24位地址
    {
        User_Reg.MB85RS2_Size[0]=0x40000;                      //256K bytes
    } else if ((User_Reg.ID[0] & 0x00000F00)==0x00000100)      //16K型号，16位地址
    {
        User_Reg.MB85RS2_Size[0]=0x4000;                       //2K bytes
    }

    MB85RS2_Size=User_Reg.MB85RS2_Size[0];

}


/*
*********************************************************************************************************
*   函 数 名: SPI_ReadWriteByte
*   功能说明: SPI字节读写
*   形    参: data：主机发送数据, index: 指定片号
*   返 回 值: 从机返回数据
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
*   函 数 名: MB85Ctrl_Write
*   功能说明: 往FRAM指定地址写数据
*   形    参: addr：要写入的地址，buf：要写入的数据指针，len：数据长度字节 index: 指定片号
*   返 回 值: 无
*********************************************************************************************************
*/
void MB85Ctrl_Write(uint32_t addr, uint8_t *buf, uint16_t len, uint8_t index)
{
    uint8_t addr_H, addr_M, addr_L;

    addr_H = (uint8_t)((addr & 0xff0000) >> 16);
    addr_M = (uint8_t)((addr & 0xff00) >> 8);
    addr_L = (uint8_t)(addr & 0x00ff);

    User_Reg.CS_MB85[index]._select();             //片选选中
    SPI_ReadWriteByte(MB85RS2_CMD_WREN, index);    //发送解除写保护指令
    User_Reg.CS_MB85[index]._diselect();           //解除片选

    User_Reg.DELAY_MB85._delyus(1);

    User_Reg.CS_MB85[index]._select();                          //片选选中
    SPI_ReadWriteByte(MB85RS2_CMD_WRITE,index);                 //发送写操作指令
    if((User_Reg.ID[index] & 0x00000F00)==0x00000800)           //2M型号,24位地址
    {
        SPI_ReadWriteByte(addr_H,index);                            //发送写地址
        SPI_ReadWriteByte(addr_M,index);
        SPI_ReadWriteByte(addr_L,index);
    } else if ((User_Reg.ID[index] & 0x00000F00)==0x00000100)    //16K型号，16位地址
    {
        SPI_ReadWriteByte(addr_M,index);                            //发送写地址
        SPI_ReadWriteByte(addr_L,index);
    }

    for(; len > 0; len--)
    {
        SPI_ReadWriteByte(*buf++,index);           //发送写数据
    }
    User_Reg.CS_MB85[index]._diselect();           //解除片选
}

/*
*********************************************************************************************************
*   函 数 名: MB85Ctrl_Read
*   功能说明: 读出FRAM指定地址数据
*   形    参: addr：要读出的地址，buf：读出的数据存储指针，len：数据长度字节 index: 指定片号
*   返 回 值: 无
*********************************************************************************************************
*/
void MB85Ctrl_Read(uint32_t addr, uint8_t *buf, uint16_t len, uint8_t index)
{
    uint8_t addr_H, addr_M, addr_L;

    addr_H = (uint8_t)((addr & 0xff0000) >> 16);
    addr_M = (uint8_t)((addr & 0xff00) >> 8);
    addr_L = (uint8_t)(addr & 0x00ff);

    User_Reg.CS_MB85[index]._select();                          //片选选中
    SPI_ReadWriteByte(MB85RS2_CMD_READ,index);                  //发送读操作指令
    if((User_Reg.ID[index] & 0x00000F00)==0x00000800)           //2M型号,24位地址
    {
        SPI_ReadWriteByte(addr_H,index);                        //发送读地址
        SPI_ReadWriteByte(addr_M,index);
        SPI_ReadWriteByte(addr_L,index);
    } else if ((User_Reg.ID[index] & 0x00000F00)==0x00000100)    //16K型号，16位地址
    {
        SPI_ReadWriteByte(addr_M,index);                         //发送读地址
        SPI_ReadWriteByte(addr_L,index);
    }
    for(; len > 0; len--)
    {
        *buf++ = SPI_ReadWriteByte(0x00, index); //按指定长度读数据
    }
    User_Reg.CS_MB85[index]._diselect();         //解除片选
}

/*
*********************************************************************************************************
*   函 数 名: MB85Ctrl_ReadID
*   功能说明: 读出FRAM厂家32位制造ID
*   形    参: buf：读出的数据存储指针 index: 指定片号
*   返 回 值: 无
*********************************************************************************************************
*/
void MB85Ctrl_ReadID(uint32_t *buf,uint8_t index)
{
    uint8_t i;
    uint8_t bytebuf[4]= {0};

    User_Reg.CS_MB85[index]._select();              //片选选中
    SPI_ReadWriteByte(MB85RS2_CMD_RDID, index);     //发送读ID操作指令
    for(i=0; i < 4; i++)
    {
        bytebuf[i] = SPI_ReadWriteByte(0x00,index); //读取ID值
    }
    *buf=(bytebuf[0]<<24)+(bytebuf[1]<<16)+(bytebuf[2]<<8)+(bytebuf[3]);  //大小端排序
    User_Reg.CS_MB85[index]._diselect();            //解除片选
}

/*
*********************************************************************************************************
*   函 数 名: MB85Ctrl_CheckWrite
*   功能说明: 往FRAM指定地址写数据,并读出检查确认写入是否正确
*   形    参: addr：要写入的地址，buf：要写入的数据指针，len：数据长度字节 index: 指定片号
*   返 回 值: 1：写入成功，0：写入失败
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
#if 0     /*使用malloc,free，可用以下代码*/
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
    /*不使用malloc,free，可用以下代码*/
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
            } //fram故障
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
