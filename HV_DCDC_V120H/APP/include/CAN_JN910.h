/*
 * CAN_JN910.h
 *
 *  Created on: 2021年6月17日
 *      Author: JC
 */

#ifndef APP_INCLUDE_CAN_JN910_H_
#define APP_INCLUDE_CAN_JN910_H_
extern struct_FILTER_COEFF Filter_Coeff;

extern struct_Active_Report     Active_Report;

extern void Init_CAN_JN910(void);
extern void CAN_BSP(CAN_APP_PACK* pack_tx);
extern void CAN_APP_RX(void);
extern void CAN_APP_TX(void);
extern void sSetProtct(void);
extern void sInquireSetProtct(void);
extern void sInquireRecondData(void);//记录数据上返
extern void sSetControlPara(void); //设置控制参数
extern void sSetCharger(void);
extern void sInquireParaData(void);
extern void sSetCalibrationPara(void);
extern void sInquireCalibrationData(void);
extern void sInquireADData(void);
extern void sSysInquireData(void);
extern void sSysSetCommand(void);
extern void sSysUpdateSet(void);
extern void sCANullCommand(void);
extern void sCRCACKCommand(void);
extern void sDebugParaSet(void);
extern void sInquireDebugPara(void);
extern void sDebugRecondData(void);
extern void sReadRecordData(void);
extern float32 Para_Var[4][12];
extern void Float2Byte(float Data,unsigned char *Ptr_Buff,Uint16 Offset_Buff);
extern Uint16 CheckSum(unsigned char *Ptr,Uint16 Offset,Uint16 Num);

extern float32 Protect_Para_Var[4][4];
extern float32 Control_Para_Var[4][12];
extern float32 Calibration_Para_Var[4][10];
extern float32 Debug_Para_Var[4][8];

extern float32 Para_Var[4][12];
extern Uint32  AD_Para_Var[4][6];
extern Uint16  Run_Mode_Var[4][1];
#endif /* APP_INCLUDE_CAN_JN910_H_ */
