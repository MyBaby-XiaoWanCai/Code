/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_eeprom.h
** Author      : Cai Heng
** Date        : 2021-06-16
** Description : 
**********************************************************************************************************/

#ifndef _BSP_EEPROM_H_
#define _BSP_EEPROM_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          ͷ�ļ�������
**********************************************************************************************************/
#include "stm32f4xx.h"

/**********************************************************************************************************
**                                            �궨����
**********************************************************************************************************/

#define bsp_InitEeprom	bsp_InitI2C		// EEPROM��ʼ��  (Configurable)

#define I2C_GPIO_CLK_ALLENABLE() {		\
		__HAL_RCC_GPIOH_CLK_ENABLE();	\
	};									// GPIOʱ��ʹ��  (Configurable)

#define I2C_SCL_GPIO	GPIOH
#define I2C_SCL_PIN		GPIO_PIN_7		// I2C_SCL : PH7 (Configurable)

#define I2C_SDA_GPIO	GPIOH
#define I2C_SDA_PIN		GPIO_PIN_8		// I2C_SDA : PH8 (Configurable)

#define I2C_SCL_0()  	I2C_SCL_GPIO->BSRR = ((uint32_t)I2C_SCL_PIN << 16U)		// SCL = 0
#define I2C_SCL_1()  	I2C_SCL_GPIO->BSRR = I2C_SCL_PIN						// SCL = 1

#define I2C_SDA_0()  	I2C_SDA_GPIO->BSRR = ((uint32_t)I2C_SDA_PIN << 16U)		// SDA = 0
#define I2C_SDA_1()  	I2C_SDA_GPIO->BSRR = I2C_SDA_PIN						// SDA = 1

#define I2C_SCL_READ()  ((I2C_SCL_GPIO->IDR & I2C_SCL_PIN) != 0)				// ��SCL����״̬
#define I2C_SDA_READ()  ((I2C_SDA_GPIO->IDR & I2C_SDA_PIN) != 0)				// ��SDA����״̬

#define I2C_GP_MODE		GPIO_MODE_OUTPUT_OD										// ��©���ģʽ (Configurable)
#define I2C_GP_PULL		GPIO_NOPULL												// ��������ʹ�� (Configurable)
#define I2C_GP_SPEED	GPIO_SPEED_HIGH											// GPIO�ٶȵȼ� (Configurable)

#define I2C_WR			0														// д����λ
#define I2C_RD			1														// ������λ

#define AT24C256						// ������������

#ifdef AT24C02
#define EE_MODEL_NAME	"AT24C02"
#define EE_DEV_ADDR		0xA0			// �豸��ַ       (Configurable)
#define EE_PAGE_SIZE	8				// ҳ���С(�ֽ�) (Configurable)
#define EE_SIZE			256				// ������(�ֽ�)   (Configurable)
#define EE_ADDR_BYTES	1				// ��ַ�ֽڸ���   (Configurable)
#endif

#ifdef AT24C128
#define EE_MODEL_NAME	"AT24C128"
#define EE_DEV_ADDR		0xA0			// �豸��ַ       (Configurable)
#define EE_PAGE_SIZE	64				// ҳ���С(�ֽ�) (Configurable)
#define EE_SIZE			16 * 1024		// ������(�ֽ�)   (Configurable)
#define EE_ADDR_BYTES	2				// ��ַ�ֽڸ���   (Configurable)
#endif

#ifdef AT24C256
#define EE_MODEL_NAME	"AT24C256"
#define EE_DEV_ADDR		0xA0			// �豸��ַ       (Configurable)
#define EE_PAGE_SIZE	64				// ҳ���С(�ֽ�) (Configurable)
#define EE_SIZE			32 * 1024		// ������(�ֽ�)   (Configurable)
#define EE_ADDR_BYTES	2				// ��ַ�ֽڸ���   (Configurable)
#endif

/*************************************************************************************************************************
**                                         β�������ַ
--------------------------------------------------------------------------------------------------------------------------
ADD��|Last_addr-6           |Last_addr-5 ~ -3      |Last_addr-2     |Last_addr-1      |Last_addr        |   
     |MacEn_flag_addr(1�ֽ�)|MacID_flag_addr(3�ֽ�)|Test_addr(1�ֽ�)|BOOT1_addr(1�ֽ�)|BOOT2_addr(1�ֽ�)|
--------------------------------------------------------------------------------------------------------------------------                      
ADD: |Last_addr-15                |Last_addr-14                |Last_addr-13              |Last_addr-12                  |                       
     |BarCode_End_flag_addr(1�ֽ�)|BarCode_Len_flag_addr(1�ֽ�)|Remote_En_flag_addr(1�ֽ�)|Remote_IPPort_flag_addr(6�ֽ�)|                      
---------------------------------------------------------------------------------------------------------------------------                        
**************************************************************************************************************************/
#define Last_addr                      EE_SIZE-1              //ĩβ�ֽڵ�ַ
#define BOOT2_addr                     Last_addr              //BOOT2��ַ
#define BOOT1_addr                     Last_addr-1            //BOOT1��ַ
#define Test_addr                      Last_addr-2            //�Լ���Ե�ַ
#define MacID_flag_addr                Last_addr-5            //MAC����3�ֽڣ��洢��ַ
#define MacEn_flag_addr                Last_addr-6            //MAC��������ʹ�ܱ�־��ַ
#define Remote_IPPort_flag_addr        Last_addr-12           //Զ��IP�˿ڴ洢��ַ
#define Remote_En_flag_addr            Last_addr-13           //Զ��IP�˿�����ʹ�ܱ�־��ַ
#define BarCode_Len_flag_addr          Last_addr-14           //���볤�ȴ洢��ַ
#define BarCode_End_flag_addr          Last_addr-15           //��������ֽڴ洢��ַ
/**********************************************************************************************************
**                                           ���Ͷ�����
**********************************************************************************************************/


/**********************************************************************************************************
**                                         �ⲿ���ñ�����
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         �ⲿ���ú�����
**********************************************************************************************************/	
extern void bsp_DelayUS(uint32_t _ulDelayTime);	

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/
