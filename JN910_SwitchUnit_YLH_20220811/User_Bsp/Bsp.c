#include "Bsp.h"

/**********************************************************************************************************
**	�� �� �� : BspInit()
**	����˵�� : �弶��ʼ��
**	��    �� : ��
**	�� �� ֵ : ��
**********************************************************************************************************/
void BspInit(void)
{
	uint16_t output = 0x0000;
	uint16_t input = 0xFFFF;

	bsp_InitDWT();	//DWT��ʱ��ʼ��
	
	ReadAddr();  //��ȡ�豸��ַ
	
	bsp_SetTIMforInt(TIM5,1000,1,0);	/*��ʼ������ʱӲ����ʱ��TIM5*/
	bsp_SetTIMforInt(TIM4,125,1,1);    /* ��������ִ�� */   
	
	SGM58601_INIT(&hspi4);  //ADC1��ʼ��
    SGM58601_INIT(&hspi2);  //ADC2��ʼ��
    SGM58601_INIT(&hspi1);  //ADC3��ʼ��
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);  //����nDRDY�ж�׼����������
	
	PCA9555_IOConfigReg(I2C_NCA9555, 0x40, &input);  //��ʼ��IOΪ����
	PCA9555_IOConfigReg(I2C_NCA9555, 0x42, &input);
	PCA9555_IOConfigReg(I2C_NCA9555, 0x44, &input);

	PCA9555_IOConfigReg(I2C_LED, 0x40, &input); 
	PCA9555_IOConfigReg(I2C_LED, 0x42, &output);       
	
	HAL_GPIO_WritePin(SNOE1_GPIO_Port, SNOE1_Pin, GPIO_PIN_RESET);	//ʹ��SN74
	HAL_GPIO_WritePin(SNOE2_GPIO_Port, SNOE2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SNOE3_GPIO_Port, SNOE3_Pin, GPIO_PIN_RESET);
}