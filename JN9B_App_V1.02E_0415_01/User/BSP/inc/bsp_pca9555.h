/**********************************************************************************************************
** Company     : JNE WuHan Co.,Ltd.
** Project     :
** --------------------------------------------------------------------------------------------------------
** File name   : bsp_pca9555.h
** Author      : Huang Cheng
** Date        : 2022-01-12
** Description : 
**********************************************************************************************************/

#ifndef _BSP_PCA9555_H_
#define _BSP_PCA9555_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************
**                                          头文件引用区
**********************************************************************************************************/
#include "main.h"

/*
	PCA9555 pin脚对应信息
	
	IO0_0 - BOOM2
	IO0_1 - BOOM1
	IO0_2 - BOOM0
	IO0_3 - PCB2
	IO0_4 - PCB1
	IO0_5 - PCB0
	IO0_6 - ID0
	IO0_7 - ID1
	
	IO1_0 - Neg_Arlm_PG
	IO1_1 - Mod_OVP_PG
	IO1_2 - Mod_OCP_PG
	IO1_3 - M_LOOP
	IO1_4 - Mod_MS
	IO1_5 - Mod_DIR
	IO1_6 - Mod_V_Range
	IO1_7 - Mod_Rev_Port
*/

/**********************************************************************************************************
**                                            宏定义区
**********************************************************************************************************/
#define PCA9555_DEVICE_ADDR            0X40

#define PCA9555_INPUT0_REG             0X00
#define PCA9555_INPUT1_REG             0X01
#define PCA9555_OUTPUT0_REG            0X02
#define PCA9555_OUTPUT1_REG            0X03
#define PCA9555_POLALN0_REG            0X04
#define PCA9555_POLALN1_REG            0X05
#define PCA9555_CONFIG0_REG            0X06
#define PCA9555_CONFIG1_REG            0X07

#define PCA9555_VERDIAL_ADDR					(0x40)
#define PCA9555_LED_ADDR							(0x42)

#define PCA9548A_DEVICE_ADDR          (0xE0)

#define PCA_WRITE			(0)
#define PCA_READ			(1)

#define PCA_9555_ON				(0)
#define PCA_9555_ALLOFF		(2)
#define PCA_9555_ALLON		(3)

#define PCA9548_CH1		(0x00)
#define PCA9548_CH2		(0x01)
#define PCA9548_CH3		(0x02)
#define PCA9548_CH4		(0x03)
#define PCA9548_CH5		(0x04)
#define PCA9548_CH6		(0x05)
#define PCA9548_CH7		(0x06)
#define PCA9548_CH8		(0x07)

/**********************************************************************************************************
**                                           类型定义区
**********************************************************************************************************/
typedef enum
{
	_OK = 0x00,
	_ERR = 0x01,
}StatusType; //状态信息

typedef struct 
{
	uint8_t WriteData[2]; //WriteData[0] - 写入 IO0_0 -> IO0_7; WriteData[1] - 写入 IO1_0 - IO1_7
	uint8_t ReadData[2]; 	//ReadData[0] - 存储	pcb版本信息; ReadData[1]- 低四位存储lm5170控制端口信息, 高四位存储故障信息
}PCA_DATA;

typedef struct
{
	PCA_DATA LED_Board[2]; 			//灯板PCA9555数据存储
	PCA_DATA Power_Board[8];		//功率板PCA9555数据存储
}PcaDataType; //PCA9548数据类型

/**********************************************************************************************************
**                                         外部引用变量区
**********************************************************************************************************/	
	
	
/**********************************************************************************************************
**                                         外部引用函数区
**********************************************************************************************************/	
StatusType PCA9555_IOConfigReg(I2C_DevType DevNum,uint16_t Pca9555Addr,uint8_t *IOArr);//PCA9555 寄存器配置

StatusType PCA9555_ConfigInputReg(I2C_DevType DevNum,uint16_t Pca9555Addr);
StatusType PCA9555_ConfigOutputReg(I2C_DevType DevNum,uint16_t Pca9555Addr);
//StatusType Pca9555_RegRead(I2C_DevType DevNum, uint8_t *RegData);
//StatusType Pca9555_RegWrite(I2C_DevType DevNum, uint8_t *RegData);
StatusType Pca9555_RegRead(I2C_DevType DevNum, uint16_t Pca9555Addr,uint8_t *RegData);
StatusType Pca9555_RegWrite(I2C_DevType DevNum, uint16_t Pca9555Addr, uint8_t *RegData);

StatusType Pca9548A_Read_Ch(I2C_DevType DevNum,uint8_t *ReadData);
StatusType Pca9548A_Select(I2C_DevType DevNum,uint8_t CHx,uint8_t Mode);
StatusType PCA_9548WRTo_9555(I2C_DevType DevNum,uint8_t CHx,uint8_t Mode,uint8_t *WriteData,uint8_t State);
StatusType Pca9555_WRTo_M_Loop(I2C_DevType DevNum,uint8_t CHx,uint8_t State,uint8_t *WriteData);
StatusType Pca_CtrlRead_Led(I2C_DevType DevNum,uint16_t Pca9555Addr, uint8_t Mode,uint8_t State,uint8_t *WriteData);	//灯板控制及信息读取

#ifdef __cplusplus
}
#endif	
	
#endif

/**********************************************************************************************************
**                                          End Of File
**********************************************************************************************************/


