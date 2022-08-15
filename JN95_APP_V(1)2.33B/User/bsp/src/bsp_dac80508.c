/**********************************************************************************************************
*
*   ģ������ : DACx0508C����
*   �ļ����� : bsp_dac80508.c
*   ��    �� : V1.0
*   ˵    �� :
*   ��   �ߣ�Robert.Li
*   ��   ��:  2021.7.26

**********************************************************************************************************/

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "bsp.h"


/**********************************************************************************************************
**                                         ȫ�ֱ���������
**********************************************************************************************************/
extern uint32_t IP_Alarm_Flag[8];       //�ڲ�������־λ
/**********************************************************************************************************
**                                           ����������
**********************************************************************************************************/
static void DAC80508_WriteCmd(uint32_t _cmd, uint8_t index);
void DAC80508_ReadCmd(uint32_t _cmd, uint8_t *Value, uint8_t index);
/**********************************************************************************************************
**                                           Ӧ�ú�����
**********************************************************************************************************/


/**********************************************************************************************************
**  �� �� �� : bsp_dac_spi_init()
**  ����˵�� : dacx0508 spi ��ʼ��
**  ��    �� : ��
**  �� �� ֵ : ��
**********************************************************************************************************/
void bsp_dac_spi_init(void)
{

    DAC1_SPI_INIT();
    DAC2_SPI_INIT();
    DAC1_CS_DISABLE();
    DAC2_CS_DISABLE();

}


/*
*********************************************************************************************************
*   �� �� ��: DACx0508C_cfg
*   ����˵��: ���� DACx0508c ������ʼ��
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void dacx0508_cfg(void)
{
    uint8_t DAC_Device_ID[2] = {0, 0};

    /* ���üĴ��� ʹ���ⲿ�ο�����ʹ��CRCУ��*/
    DAC80508_WriteCmd((3 << 16) | 0x100, dac_1);

    /* ����Ĵ��� �ο���ѹ2.5V����2 */
    DAC80508_WriteCmd((4 << 16) | 0xFF, dac_1);

    /* ���üĴ��� ʹ���ⲿ�ο�����ʹ��CRCУ��*/
    DAC80508_WriteCmd((3 << 16) | 0x100, dac_2);

    /* ����Ĵ��� �ο���ѹ2.5V����2 */
    DAC80508_WriteCmd((4 << 16) | 0xFF, dac_2);

    /*��DAC1IDֵ*/
    DAC80508_ReadCmd(0x810000, DAC_Device_ID, dac_1);
    //#if DACx0508C_PRINTF_CFG
    //    printf("\r\ndac1_ID:%x,%x\r\n", DAC_Device_ID[0], DAC_Device_ID[1]);
    //#endif

    if(!((DAC_Device_ID[0] == 0x08) && ((DAC_Device_ID[1] & 0xfc) == 0x94)))

    {
                             for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_DAE;  //DAC����
    }

    /*��DAC2IDֵ*/
    DAC80508_ReadCmd(0x810000, DAC_Device_ID, dac_2);
    if(!((DAC_Device_ID[0] == 0x08) && ((DAC_Device_ID[1] & 0xfc) == 0x94)))

    {
                             for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_DAE;  //DAC����
    }

#if DACx0508C_PRINTF_CFG
    printf("\r\ndac2_ID:%x,%x\r\n", DAC_Device_ID[0], DAC_Device_ID[1]);
#endif


    /* ��λ2��DAC ���0V */
    DAC80508_BRDCAST(0x0000, 0);

    DAC80508_BRDCAST(0x0000, 1);

}


/*
*********************************************************************************************************
*   �� �� ��: bsp_dacx0508_init
*   ����˵��: DACx0508c ��ʼ�����ú�����bsp_init ����
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/


void bsp_dacx0508_init(void)
{
    bsp_dac_spi_init();
    dacx0508_cfg();
}




/*
*********************************************************************************************************
*   �� �� ��: SPI4_ReadWriteByte
*   ����˵��: SPI�ֽڶ�д
*   ��    ��: data��������������
*   �� �� ֵ: �ӻ���������
*********************************************************************************************************
*/
static uint8_t SPI4_ReadWriteByte(uint8_t data)
{
    uint8_t Rx_data = 0;
    HAL_SPI_TransmitReceive(&SPI4_Handler, &data, &Rx_data, 1, 0xff);
    return Rx_data;
}


/*
*********************************************************************************************************
*   �� �� ��: SPI5_ReadWriteByte
*   ����˵��: SPI�ֽڶ�д
*   ��    ��: data��������������
*   �� �� ֵ: �ӻ���������
*********************************************************************************************************
*/
static uint8_t SPI5_ReadWriteByte(uint8_t data)
{
    uint8_t Rx_data = 0;
    HAL_SPI_TransmitReceive(&SPI5_Handler, &data, &Rx_data, 1, 0xff);
    return Rx_data;
}


/*
*********************************************************************************************************
*   �� �� ��: DAC80508_WriteCmd
*   ����˵��: ��SPI���߷���24��bit���ݡ�
*   ��    ��: _cmd : ���� index:�豸���
*   �� �� ֵ: ��
*********************************************************************************************************
*/

void DAC80508_WriteCmd(uint32_t _cmd, uint8_t index)
{
    uint8_t Txdata[3] = {_cmd >> 16, (_cmd >> 8) & 0x00FF, _cmd & 0x0000FF};
    uint8_t Rxdata[3] = {0, 0, 0};
    int32_t ret = 0;
    if(index == dac_1)
    {
        DAC1_CS_ENABLE();
        ret = HAL_SPI_TransmitReceive(&DAC1_HANDLER, Txdata, Rxdata, 3, 1000);
        DAC1_CS_DISABLE();
    }
    else if(index == dac_2)
    {
        DAC2_CS_ENABLE();
        ret = HAL_SPI_TransmitReceive(&DAC2_HANDLER, Txdata, Rxdata, 3, 1000);
        DAC2_CS_DISABLE();
    }

    if(ret == HAL_ERROR)
    {
                             for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_DAE;  //DAC����
        ERROR_HANDLER();
    }
}

void delayn(int32_t n)
{
    while(n > 0)
    {
        n--;
    }
}







/*********************************************************************************************************
*   �� �� ��: DAC80508_ReadCmd
*   ����˵��: ��SPI���߷���24��bit���ݡ�
*   ��    ��: _cmd : ���� index:�豸���
*   �� �� ֵ: ��
*********************************************************************************************************
*/

void DAC80508_ReadCmd(uint32_t _cmd, uint8_t *Value, uint8_t index)
{
    uint8_t Txdata[3] = {_cmd >> 16, (_cmd >> 8) & 0x00FF, _cmd & 0x0000FF};
    uint8_t i = 0;
    uint8_t j;
    if(index == 0)
	{
		for(j = 0; j < 2; j++)
		{
			DAC1_CS_ENABLE();
			SPI4_ReadWriteByte(Txdata[0]);

			for(i = 0; i < 2; i++)
			{
				Value[i] = SPI4_ReadWriteByte(0x00);
			}

			DAC1_CS_DISABLE();
		}    
    }
    else
    {
		for(j = 0; j < 2; j++)
		{
			DAC2_CS_ENABLE();
			SPI5_ReadWriteByte(Txdata[0]);
			
			for(i = 0; i < 2; i++)
			{
				Value[i] = SPI5_ReadWriteByte(0x00);
			}

			DAC2_CS_DISABLE();
		}
    }
}

/////*
////*********************************************************************************************************
////*   �� �� ��: DAC80508_ReadCmd
////*   ����˵��: ��SPI���߷���24��bit���ݡ�
////*   ��    ��: _cmd : ���� index:�豸���
////*   �� �� ֵ: ��
////*********************************************************************************************************
////*/

////void DAC80508_ReadCmd(uint32_t _cmd, uint8_t *Value, uint8_t index)
////{
////	HAL_StatusTypeDef ret;
////    uint8_t Txdata[3] = {_cmd >> 16, (_cmd >> 8) & 0x00FF, _cmd & 0x0000FF};
////    uint8_t Rxdata[3] = {0, 0, 0};

////    uint8_t i = 0;

////    if(index == 0)
////    {
////        for(i = 0; i < 2; i++)
////        {
////            DAC1_CS_ENABLE();

////            ret=HAL_SPI_TransmitReceive(&DAC2_HANDLER, Txdata, Rxdata, 3, 1000);
////            if(HAL_OK !=ret )
////            {
////                ERROR_HANDLER();
////            }
////            DAC1_CS_DISABLE();
////        }
////    }
////    else
////    {
////        for(i = 0; i < 2; i++)
////        {
////            DAC2_CS_ENABLE();
////            if(HAL_OK != HAL_SPI_TransmitReceive(&DAC2_HANDLER, Txdata, Rxdata, 3, 1000))
////            {
////                ERROR_HANDLER();
////            }
////            DAC2_CS_DISABLE();
////        }
////    }


////    for(i = 0; i < 3; i++)
////    {
////        Value[i] = Rxdata[i + 1];
////    }


////}


/*
*********************************************************************************************************
*   �� �� ��: DAC80508_SetDacData
*   ����˵��: ����DAC���,��ͨ���������¡�
*   ��    ��: _ch, ͨ��, 0 , 1
*            _data : ���� index���豸����
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void    DAC80508_SetDacData(uint8_t _ch, uint16_t _dac, uint8_t index)
{

    DAC80508_WriteCmd(((0x8 + _ch) << 16) | _dac, index);

}


/*
*********************************************************************************************************
*   �� �� ��: DAC_SetLinearData
*   ����˵��: ������������DAC���
*   ��    ��: _ch, ͨ��, 0 , 1
*             _Target_Iout��Ŀ�����  index���豸����
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void DAC_SetLinearData(uint8_t _ch, uint16_t _Target_Iout, uint8_t index)
{

    static uint32_t DACX_ISET = 0;
    uint8_t DACX_Iout[2] = {0,0};

    DAC80508_ReadDacData(_ch, DACX_Iout, index);
    DACX_ISET = DACX_Iout[0] << 8 | DACX_Iout[1];

    if(DACX_ISET < _Target_Iout) //��������Ŀ��ֵ������ڳ�ʼֵ
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

    else            //�½�ģʽ�£���ʼֻ��������趨ֵ
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
*   �� �� ��: DAC_SetLinearData
*   ����˵��: ������������DAC���
*   ��    ��: _ch, ͨ��, 0 , 1
*             _Target_Iout��Ŀ�����  index���豸����
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void DAC_SetLinearData_2channl(uint8_t _ch1, uint8_t _ch2, uint16_t _Target_Iout1, uint16_t _Target_Iout2, uint8_t index)
{

    static uint32_t DACX_ISET_Ch1, DACX_ISET_Ch2;
    uint8_t DACX_Iout[2] = {0, 0};

    DACX_ISET_Ch1 = 0;
    DACX_ISET_Ch2 = 0;

//    DAC80508_ReadDacData(_ch1, DACX_Iout, index);
    DACX_ISET_Ch1 = DACX_Iout[0] << 8 | DACX_Iout[1];

//    DAC80508_ReadDacData(_ch2, DACX_Iout, index);
    DACX_ISET_Ch2 = DACX_Iout[0] << 8 | DACX_Iout[1];

    if(DACX_ISET_Ch1 < _Target_Iout1) //��������Ŀ��ֵ������ڳ�ʼֵ
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

    else            //�½�ģʽ�£���ʼֻ��������趨ֵ
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
*   �� �� ��: DAC80508_Read_DAC_Value
*   ����˵��: DAC80508��DAC ����ֵ
*   ��    ��:
*
*   �� �� ֵ: ��
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
        DAC1_CS_ENABLE();
        ret = HAL_SPI_TransmitReceive(&DAC1_HANDLER, Txdata, Rxdata, size, 1000);
        DAC1_CS_DISABLE();
    }
    else
    {
        DAC2_CS_ENABLE();
        ret = HAL_SPI_TransmitReceive(&DAC2_HANDLER, Txdata, Rxdata, size, 1000);
        DAC2_CS_DISABLE();
    }

    /* Build the result */
    for(i = 0; i < size; i++)
    {
        Value[i] = Rxdata[i];
    }
    if(ret != HAL_OK)
    {
        ERROR_HANDLER();
    }


}



/*
*********************************************************************************************************
*   �� �� ��: DAC8562_ReadDacData
*   ����˵��: ����DAC���,��ͨ���������¡�
*   ��    ��: _ch, ͨ��, 0 , 1
*            _data : ���� index���豸����
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void DAC80508_ReadDacData(uint8_t _ch, uint8_t *Value, uint8_t index)
{
    DAC80508_ReadCmd(((0x88 + _ch) << 16) , Value , index);
}



/*
*********************************************************************************************************
*   �� �� ��: DAC80508_BRDCAST
*   ����˵��: ����DAC���,�㲥���ͣ�ֵ��Ҫһ��
*   ��    ��: _dac��ֵ index���豸����
*
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void DAC80508_BRDCAST(uint16_t _dac, uint8_t index)
{
    DAC80508_WriteCmd((0x6 << 16) | _dac, index);
}



/**********************************************************************************************************
**  �� �� �� :
**  ����˵�� :
**  ��    �� :
**  �� �� ֵ :
**********************************************************************************************************/


/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
