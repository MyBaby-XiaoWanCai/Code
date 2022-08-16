/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_MB85RS2.h
** Author      : Huang Cheng
** Date        : 2021-11-09
** Description : bsp_MB85RS2 head file
**********************************************************************************************************/

#ifndef _BSP_MB85RS2_H_
#define _BSP_MB85RS2_H_

#ifdef __cplusplus
extern "C" {
#endif

    /**********************************************************************************************************
    **                                          ͷ�ļ�������
    **********************************************************************************************************/
#include "main.h"


    /**********************************************************************************************************
    **                                            �궨����
    **********************************************************************************************************/
#define ALRM_MB85_EN               0         //ʹ�ܹ��ϱ���
#define MX_NUM                     5         //���ʹ���豸������Ĭ��5Ƭ

#define MB85RS2_CMD_WREN 		   0x06      //дʹ��
#define MB85RS2_CMD_WRDI		   0x04      //д��ֹ
#define MB85RS2_CMD_RDSR		   0x05      //��״̬�Ĵ���
#define MB85RS2_CMD_WRSR		   0x01      //д״̬�Ĵ���
#define MB85RS2_CMD_READ		   0x03      //24λ��ַģʽ������ 
#define MB85RS2_CMD_WRITE		   0x02      //д����
#define MB85RS2_CMD_FSTRD      0x0B      //32λ��ַģʽ����λ���룬��λ��0��������
#define MB85RS2_CMD_RDID		   0x9f      //��ID��32λID
#define MB85RS2_CMD_SLEEP		   0xB9      //�͹���ģʽ

#define StartADD_WorkStatus	       0          // ��һ�ζϵ繤��״̬��¼��������ʼ��ַ��8�ֽڣ�
#define StartADD_MB85RS2DeviceSN   8          // �豸SN�����ţ�3�ֽڣ���ַ

#define APP_Copy_Flag_ADD        MB85RS2_Size-1   //�������һ����ַ��ΪAPP������־��ַ 
#define JUMP_FCT_Flag_ADD        MB85RS2_Size-4   //���絹����4�ֽ���ΪFCT������ת��־
#define MB85R_SPI_UNIT           (M4_SPI6)
#define MB85R_NSS_PORT           (GPIO_PORT_B)   //ƬѡIO����
#define MB85R_NSS_PIN            (GPIO_PIN_15)
#define MB85R_SCK_PORT           (GPIO_PORT_G)
#define MB85R_SCK_PIN            (GPIO_PIN_13)
#define MB85R_SCK_FUNC           (GPIO_FUNC_46_SPI6_SCK)
#define MB85R_MISO_PORT          (GPIO_PORT_G)
#define MB85R_MISO_PIN           (GPIO_PIN_12)
#define MB85R_MISO_FUNC          (GPIO_FUNC_48_SPI6_MISO)
#define MB85R_MOSI_PORT          (GPIO_PORT_G)
#define MB85R_MOSI_PIN           (GPIO_PIN_14)
#define MB85R_MOSI_FUNC          (GPIO_FUNC_47_SPI6_MOSI)

    /**********************************************************************************************************
    **                                           ���Ͷ�����
    **********************************************************************************************************/
    
    /*SPIƬѡ����ע������*/
    typedef  struct _CS0_MB85
    {
        void (*_select)(void);	 ///< Ƭѡ0ѡ��
        void (*_diselect)(void); ///< Ƭѡ0���

    }CS0_MB85;

    /*SPI�ӿںͳ�ʼ������ע������*/

    typedef  struct _SPI0_MB85
    {
        uint32_t (*_tran_re_byte)(const void * wb,void *rb,uint32_t len);   ///д������bytes����
        void (*_init)(void);                   ///�ӿڳ�ʼ������
    }SPI0_MB85;

    /*�û�ע�ắ�����ͽṹ��*/
    typedef  struct __UserFunMB85_Type
    {
        uint32_t ID[5];               //�����豸ID
        uint32_t MB85RS2_Size[5];     //�����豸�洢�ռ��С
        
        /**
         * ��ʱ����ע������
         */
        struct  _DELAY_MB85
        {
            void (*_delyus)(uint32_t us); ///< ��ʱ����US
            void (*_delyms)(uint32_t ms); ///< ��ʱ����MS
        } DELAY_MB85;


        /**
         * �ٽ籣������ע������
         */
         struct  _CRIS0_MB85
        {
            void (*_enter)(void); ///< �ٽ籣�������뺯��
            void (*_exit)(void);  ///< �ٽ籣�����˳�����
        } CRIS0_MB85;


        CS0_MB85 CS_MB85[MX_NUM];    //��Զ�ƬӦ�ã�ʹ�ýṹ������洢����ָ��

        SPI0_MB85 SPI_MB85[MX_NUM];    //��Զ�ƬӦ�ã�ʹ�ýṹ������洢����ָ��

    } UserFunMB85_Type;


    /**********************************************************************************************************
    **                                         �ⲿ���ñ�����
    **********************************************************************************************************/
   extern uint32_t MB85RS2_Size;

    /**********************************************************************************************************
    **                                         �ⲿ���ú�����
    **********************************************************************************************************/
    void MB85Ctrl_Read(uint32_t addr, uint8_t *buf, uint16_t len, uint8_t index);
    void MB85Ctrl_Write(uint32_t addr, uint8_t *buf, uint16_t len, uint8_t index);
    void MB85Ctrl_ReadID(uint32_t *buf,uint8_t index);
    uint8_t MB85Ctrl_CheckWrite(uint32_t addr, uint8_t *buf, uint16_t len, uint8_t index);
    void bsp_InitMB85R(void);
#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
