/*
 * CAN_JN910.h
 *
 *  Created on: 2021年6月17日
 *      Author: JC
 */

#ifndef APP_INCLUDE_CAN_JN910_H_
#define APP_INCLUDE_CAN_JN910_H_

extern struct_CAN_JN910_TX      CAN_JN910_TX;
extern struct_CAN_JN910_RX      CAN_JN910_RX;
extern struct_Active_Report     Active_Report;
extern struct_CAN_JN910_RX      CAN_JN910_RX_Broadcast;
extern struct_CAN_JN910_TX      CAN_JN910_TX_Error;
extern void Init_CAN_JN910(void);
extern void CAN_BSP(struct_CAN_JN910_TX * CAN_JN910_TX);
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
extern void sSysDebug(void);
#endif /* APP_INCLUDE_CAN_JN910_H_ */
