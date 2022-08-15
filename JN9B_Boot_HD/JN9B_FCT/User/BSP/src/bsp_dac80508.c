/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_dac80508.c
** Author      : Huang Cheng
** Date        : 2021-11-05
** Description : bsp_dac80508 source file
**********************************************************************************************************/

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "bsp_dac80508.h"
#include "bsp_spi.h"
#include "bsp.h"

/**********************************************************************************************************
**                                         全局变量定义区
**********************************************************************************************************/


/**********************************************************************************************************
**                                           函数声明区
**********************************************************************************************************/
static void bsp_dac_spi_init(void);
static void dacx0508_cfg(void);
static uint8_t SPI4_ReadWriteByte(uint8_t data);
static uint8_t SPI5_ReadWriteByte(uint8_t data);
static void DAC80508_WriteCmd(uint32_t _cmd, uint8_t index);
static void DAC80508_ReadCmd(uint32_t _cmd, uint8_t *Value, uint8_t index);

/**********************************************************************************************************
**                                           应用函数区
**********************************************************************************************************/

/**********************************************************************************************************
**  函 数 名 : bsp_dac_spi_init()
**  功能说明 : dacx0508 spi 初始化
**  形    参 : 无
**  返 回 值 : 无
**********************************************************************************************************/
static void bsp_dac_spi_init(void)
{
    DAC1_SpiInit();
    DAC2_SpiInit();
}

/*********************************************************************************************************
*   函 数 名: DACx0508C_cfg
*   功能说明: 配置 DACx0508c 参数初始化
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************/
static void dacx0508_cfg(void)
{
    uint8_t DAC_Device_ID[2] = {0, 0};

    /* 配置寄存器 使用外部参考，不使用CRC校验*/
    DAC80508_WriteCmd((3 << 16) | 0x100, dac_1);

    /* 增益寄存器 参考电压2.5V乘以2 */
    DAC80508_WriteCmd((4 << 16) | 0xFF, dac_1);

    /* 配置寄存器 使用外部参考，不使用CRC校验*/
    DAC80508_WriteCmd((3 << 16) | 0x100, dac_2);

    /* 增益寄存器 参考电压2.5V乘以2 */
    DAC80508_WriteCmd((4 << 16) | 0xFF, dac_2);

    /*读DAC1ID值*/
    DAC80508_ReadCmd(0x810000, DAC_Device_ID, dac_1);

    if(!((DAC_Device_ID[0] == 0x08) && ((DAC_Device_ID[1] & 0xfc) == 0x94)))
    {
		for(uint8_t j = 0; j < CHANNEL_CFG; j++)
		{
			IP_Alarm_Flag[j] |= IntelProtect_DAE;  //DAC故障
		}
    }

    /*读DAC2ID值*/
    DAC80508_ReadCmd(0x810000, DAC_Device_ID, dac_2);
    if(!((DAC_Device_ID[0] == 0x08) && ((DAC_Device_ID[1] & 0xfc) == 0x94)))
    {
		for(uint8_t j = 0; j < CHANNEL_CFG; j++)
		{
			IP_Alarm_Flag[j] |= IntelProtect_DAE;  //DAC故障
		}
    }

#if DACx0508C_PRINTF_CFG
    printf("\r\ndac2_ID:%x,%x\r\n", DAC_Device_ID[0], DAC_Device_ID[1]);
#endif
	
    /* 复位2个DAC 输出0V */
    DAC80508_BRDCAST(0x0000, 0);
    DAC80508_BRDCAST(0x0000, 1);

}


/*********************************************************************************************************
*   函 数 名: bsp_dacx0508_init
*   功能说明: DACx0508c 初始化，该函数被bsp_init 调用
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************/
void bsp_dacx0508_init(void)
{
    bsp_dac_spi_init();
    dacx0508_cfg();
}

/*
*********************************************************************************************************
*   函 数 名: SPI4_ReadWriteByte
*   功能说明: SPI字节读写
*   形    参: data：主机发送数据
*   返 回 值: 从机返回数据
*********************************************************************************************************
*/
static uint8_t SPI4_ReadWriteByte(uint8_t data)
{
    uint8_t Rx_data = 0;
    SPI_TransmitReceive(DAC1_SPI_UNIT, &data, &Rx_data, 1);
    return Rx_data;
}


/*********************************************************************************************************
*   函 数 名: SPI5_ReadWriteByte
*   功能说明: SPI字节读写
*   形    参: data：主机发送数据
*   返 回 值: 从机返回数据
*********************************************************************************************************/
static uint8_t SPI5_ReadWriteByte(uint8_t data)
{
    uint8_t Rx_data = 0;
    SPI_TransmitReceive(DAC2_SPI_UNIT, &data, &Rx_data, 1);
    return Rx_data;
}


/*********************************************************************************************************
*   函 数 名: DAC80508_WriteCmd
*   功能说明: 向SPI总线发送24个bit数据。
*   形    参: _cmd : 数据 index:设备编号
*   返 回 值: 无
*********************************************************************************************************/
static void DAC80508_WriteCmd(uint32_t _cmd, uint8_t index)
{
    uint8_t Txdata[3] = {_cmd >> 16, (_cmd >> 8) & 0x00FF, _cmd & 0x0000FF};
    uint8_t Rxdata[3] = {0, 0, 0};
    int32_t ret = 0;
	
    if(index == dac_1) //写DAC1的数据
    {
        DAC1_NSS_0();
        ret = SPI_TransmitReceive(DAC1_SPI_UNIT, Txdata, Rxdata, 3);
        DAC1_NSS_1();
    }
    else if(index == dac_2) //写DAC2的数据
    {
        DAC2_NSS_0();
        ret = SPI_TransmitReceive(DAC2_SPI_UNIT, Txdata, Rxdata, 3);
        DAC2_NSS_1();
    }

    if(ret != Ok)
    {
        for(uint8_t j = 0; j < CHANNEL_CFG; j++)
		{
			IP_Alarm_Flag[j] |= IntelProtect_DAE;  //DAC故障
			ERROR_HANDLER();
		}
    }
}

/*********************************************************************************************************
*   函 数 名: DAC80508_ReadCmd
*   功能说明: 向SPI总线发送24个bit数据。
*   形    参: _cmd : 数据 index:设备编号
*   返 回 值: 无
**********************************************************************************************************/
static void DAC80508_ReadCmd(uint32_t _cmd, uint8_t *Value, uint8_t index)
{
    uint8_t Txdata[3] = {_cmd >> 16, (_cmd >> 8) & 0x00FF, _cmd & 0x0000FF};
    uint8_t i = 0;
    uint8_t j;
	
    if(index == 0)  //读DAC1的数据，此芯片需要读取2次，第2次数据才有效
	{
		for(j = 0; j < 2; j++)
		{
			DAC1_NSS_0();
			SPI4_ReadWriteByte(Txdata[0]);

			for(i = 0; i < 2; i++)
			{
				Value[i] = SPI4_ReadWriteByte(0x00);
			}

			DAC1_NSS_1();
		}    
    }
    else           //读DAC2的数据，此芯片需要读取2次，第2次数据才有效
    {
		for(j = 0; j < 2; j++)
		{
			DAC2_NSS_0();
			SPI5_ReadWriteByte(Txdata[0]);
			
			for(i = 0; i < 2; i++)
			{
				Value[i] = SPI5_ReadWriteByte(0x00);
			}

			DAC2_NSS_1();
		}
    }
}

/*
*********************************************************************************************************
*   函 数 名: DAC80508_SetDacData
*   功能说明: 设置DAC输出,单通道立即更新。
*   形    参: _ch: 通道（0-7），_data : 数据，index：设备索引
*   返 回 值: 无
*********************************************************************************************************
*/
void DAC80508_SetDacData(uint8_t _ch, uint16_t _dac, uint8_t index)
{
    DAC80508_WriteCmd(((0x8 + _ch) << 16) | _dac, index);
}

/*
*********************************************************************************************************
*   函 数 名: DAC_SetLinearData
*   功能说明: 设置上升线性DAC输出
*   形    参: _ch, 通道, 0 , 1
*             _Target_Iout：目标电流  index：设备索引
*   返 回 值: 无
*********************************************************************************************************
*/
void DAC_SetLinearData(uint8_t _ch, uint16_t _Target_Iout, uint8_t index)
{

    static uint16_t DACX_ISET = 0;
    uint8_t DACX_Iout[2] = {0,0};

    DAC80508_ReadDacData(_ch, DACX_Iout, index);
    DACX_ISET = DACX_Iout[0] << 8 | DACX_Iout[1];

    if(DACX_ISET < _Target_Iout) //上升设置目标值必须大于初始值
    {

        for(; DACX_ISET < _Target_Iout; DACX_ISET += 20)
        {
            DAC80508_SetDacData(_ch, DACX_ISET, index);
        }
        if(DACX_ISET >= _Target_Iout)
        {
            DAC80508_SetDacData(_ch, _Target_Iout, index);
        }
    }

    else            //下降模式下，初始只必须大于设定值
    {

        for(; DACX_ISET > _Target_Iout; DACX_ISET -= 40)
        {

            DAC80508_SetDacData(_ch, DACX_ISET, index);
            if(DACX_ISET < 40)
            {
                break;
            }

        }
        DAC80508_SetDacData(_ch, _Target_Iout, index);
    }

}


/*
*********************************************************************************************************
*   函 数 名: DAC_SetLinearData
*   功能说明: 设置上升线性DAC输出
*   形    参: _ch, 通道, 0 , 1
*             _Target_Iout：目标电流  index：设备索引
*   返 回 值: 无
*********************************************************************************************************
*/
void DAC_SetLinearData_2channl(uint8_t _ch1, uint8_t _ch2, uint16_t _Target_Iout1, uint16_t _Target_Iout2, uint8_t index)
{

    static uint16_t DACX_ISET_Ch1, DACX_ISET_Ch2;
    uint8_t DACX_Iout[2] = {0, 0};

    DACX_ISET_Ch1 = 0;
    DACX_ISET_Ch2 = 0;

//    DAC80508_ReadDacData(_ch1, DACX_Iout, index);
    DACX_ISET_Ch1 = DACX_Iout[0] << 8 | DACX_Iout[1];

//    DAC80508_ReadDacData(_ch2, DACX_Iout, index);
    DACX_ISET_Ch2 = DACX_Iout[0] << 8 | DACX_Iout[1];

    if(DACX_ISET_Ch1 < _Target_Iout1) //上升设置目标值必须大于初始值
    {

        for(; DACX_ISET_Ch1 < _Target_Iout1; DACX_ISET_Ch1 += 40)
        {
            DAC80508_SetDacData(_ch1, DACX_ISET_Ch1, index);

            if(DACX_ISET_Ch2 < _Target_Iout1)
            {
                DAC80508_SetDacData(_ch2, DACX_ISET_Ch2, index);
                DACX_ISET_Ch2 += 40;
            }
        }

        if(DACX_ISET_Ch1 >= _Target_Iout1)
        {
            DAC80508_SetDacData(_ch1, _Target_Iout1, index);
            DAC80508_SetDacData(_ch2, _Target_Iout2, index);

        }
    }

    else            //下降模式下，初始只必须大于设定值
    {

        for(; DACX_ISET_Ch1 > _Target_Iout1; DACX_ISET_Ch1 -= 60)
        {

            DAC80508_SetDacData(_ch1, DACX_ISET_Ch1, index);
            DAC80508_SetDacData(_ch2, DACX_ISET_Ch1, index);
            if(DACX_ISET_Ch1 < 60)
            {
                break;
            }

        }
        DAC80508_SetDacData(_ch1, _Target_Iout1, index);
        DAC80508_SetDacData(_ch2, _Target_Iout2, index);
    }

}

/*
*********************************************************************************************************
*   函 数 名: DAC80508_Read_DAC_Value
*   功能说明: DAC80508读DAC 的数值
*   形    参: Addr - 寄存器地址， *Value - 返回的DAC数据，size - 返回数据字节长度，index - 设备地址
*   返 回 值: 无
*********************************************************************************************************
*/
void DAC80508_Read_DAC_Value(uint8_t Addr, uint8_t *Value, uint8_t size, uint8_t index)
{

    uint8_t Txdata[3] = {0, 0, 0};
    uint8_t Rxdata[3] = {0, 0, 0};
    int32_t ret = 0;
    uint8_t i = 0;


    /* Build the Read command word */
    //    Txdata[2] = DACx0508C_READ | Addr;
    Txdata[0] = DACx0508C_READ | Addr;

    if(index == 0)
    {
        DAC1_NSS_0();
        ret = SPI_TransmitReceive(DAC1_SPI_UNIT, Txdata, Rxdata, size);
        DAC1_NSS_1();
    }
    else
    {
        DAC2_NSS_0();
        ret = SPI_TransmitReceive(DAC2_SPI_UNIT, Txdata, Rxdata, size);
        DAC2_NSS_1();
    }

    /* Build the result */
    for(i = 0; i < size; i++)
    {
        Value[i] = Rxdata[i];
    }
    if(ret != Ok)
    {
        ERROR_HANDLER();
    }
}

/*
*********************************************************************************************************
*   函 数 名: DAC8562_ReadDacData
*   功能说明: 设置DAC输出,单通道立即更新。
*   形    参: _ch, 通道, 0 , 1
*            _data : 数据 index：设备索引
*   返 回 值: 无
*********************************************************************************************************
*/
void DAC80508_ReadDacData(uint8_t _ch, uint8_t *Value, uint8_t index)
{
    DAC80508_ReadCmd(((0x88 + _ch) << 16) , Value , index);
}

/*
*********************************************************************************************************
*   函 数 名: DAC80508_BRDCAST
*   功能说明: 设置DAC输出,广播发送，值需要一样
*   形    参: _dac：值 index：设备索引
*   返 回 值: 无
*********************************************************************************************************
*/
void DAC80508_BRDCAST(uint16_t _dac, uint8_t index)
{
    DAC80508_WriteCmd((0x6 << 16) | _dac, index);
}


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/




