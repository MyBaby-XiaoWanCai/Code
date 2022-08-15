#ifndef _CLA_SHARED_H_
#define _CLA_SHARED_H_
//#############################################################################
//
// FILE:   cla_sqrt_shared.h
//
// TITLE:  Square Root Test
//
//#############################################################################
// $TI Release: F2837xD Support Library v210 $
// $Release Date: Tue Nov  1 14:46:15 CDT 2016 $
// $Copyright: Copyright (C) 2013-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//#############################################################################

//
// Included Files
//
#include "F2837xS_Cla_defines.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct Channel
{
     int32 Set_Cur;                     //��ǰͨ��Ŀ�����
    Uint16 EN_SynRect;                  //ͬ������ʹ�ܱ�־




    Uint16 Cnt_FastStart;               //����������ʱ������
    //ģ���ڲ��������ⲿ����Ҫ����
    Uint16 Cnt_SynRect;                 //ͬ������������ʱ������
    Uint16 Flag_SynRect;                //ͬ��������������־
    Uint16 Cnt_SynRect_Off;             //ͬ�������ر���ʱ����
    Uint16 PWMxA;                       //��ǰ����ͨ��PWMxA��ռ�ձ�
    Uint16 PWMxAHR;
    Uint16 PWMxB;                       //��ǰ����ͨ��PWMxB��ռ�ձ�
    Uint16 PWMxBHR;
    int32 Now_Cur;                     //��ǰͨ������ת����ĵ���ֵ
    Uint16 Cnt_Out_OC;
    Uint16 Flag_MOS_Out;                //���MOS����״̬��־

    Uint16 Req_FastStart;             //������������
    Uint16 Req_FastStart_DCM;              //������������
} Channel;

// State machine typedef for ENC status
typedef enum
{
    ENC_ALIGNMENT        = 0,
    ENC_WAIT_FOR_INDEX   = 1,
    ENC_CALIBRATION_DONE = 2
} ENC_Status_e;
typedef struct _FCL_Vars_t_
{
    ENC_Status_e lsw;
    PI_CLA CH_A_PI_Cur;
    PI_CLA CH_B_PI_Cur;
    PI_CLA CH_PI_Iout;
    Uint16 Empty_Full_Flag;
    Uint16 Once_Flag;
    Uint16 Neg_Full_Flag;
    float Isense_Curr;
   // QEP qep;
    //volatile struct EQEP_REGS *ptrQEP;  // Aligned to lower 16-bits
    uint32_t taskCount;
} FCL_Vars_t;

typedef struct struct_ADCA_Sample
{
    float ADCA_CH_SUM0;
    float ADCA_CH_SUM1;
    float ADCA_CH_SUM2;
    float ADCA_CH_SUM3;
    float ADCA_CH_SUM4;
    float ADCA_CH_SUM5;
    float ADCA_CH_SUM6;
    float ADCA_CH_SUM7;
    float ADC0;
    float ADC1;
    float ADC2;
    float ADC3;
    float ADC4;
    float ADC5;
    float ADC6;
    float ADC7;
    Uint16 resultsIndex1;
    Uint16 resultsIndex2;
    Uint16 resultsIndex3;
    Uint16 resultsIndex4;
    float ADCA_CH0[5];
    float ADCA_CH1[5];
    float ADCA_CH2[5];
    float ADCA_CH3[5];
    float ADCA_CH4[5];
    float ADCA_CH5[5];
    float ADCA_CH6[5];
    float ADCA_CH7[5];
}struct_ADCA_Sample;

extern FCL_Vars_t fclVars[4];
extern struct_ADCA_Sample ADCA_Sample;
//
// Globals
//

//
//Task 1 (C) Variables
//
extern float fVal,fVal2; //input
extern float fResult,fResult2;  //Estimated result
extern volatile float IdRef[4];
extern volatile float PWM_Ready[4];
extern volatile uint32_t  MEP_Scale;
//
//Task 2 (C) Variables
//

//
//Task 3 (C) Variables
//

//
//Task 4 (C) Variables
//

//
//Task 5 (C) Variables
//

//
//Task 6 (C) Variables
//

//
//Task 7 (C) Variables
//

//
//Task 8 (C) Variables
//

//
//Common (C) Variables
//

//
// Function Prototypes
//
// The following are symbols defined in the CLA assembly code
// Including them in the shared header file makes them
// .global and the main CPU can make use of them.
//
__interrupt void Cla1Task1();
__interrupt void Cla1Task2();
__interrupt void Cla1Task3();
__interrupt void Cla1Task4();
__interrupt void Cla1Task5();
__interrupt void Cla1Task6();
__interrupt void Cla1Task7();
__interrupt void Cla1Task8();

#ifdef __cplusplus
}
#endif // extern "C"

#endif //end of _CLA_SQRT_SHARED_H_ definition

//
// End of file
//
