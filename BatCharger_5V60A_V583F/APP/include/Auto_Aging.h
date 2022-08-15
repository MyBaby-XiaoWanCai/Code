/*
 * Auto_Aging.h
 *
 *  Created on: 2020��6��30��
 *      Author: jc
 */

#ifndef APP_INCLUDE_AUTO_AGING_H_
#define APP_INCLUDE_AUTO_AGING_H_



//�ṹ�嶨��
typedef struct struct_Auto_Aging
{
    Uint16 State;
    Uint32 Cnt_Delay;

    Uint16 Req_WR_M1;
    Uint16 Req_WR_M2;
    Uint16 State_WritetoFalsh;

    Uint16 Flag_CV_Fail;
    Uint16 Cnt_CV_Fail;

    Uint16 Step_Aging;//�ϻ�����״̬
    Uint16 CurDiff_Aging;
} struct_Auto_Aging;

#define STEP_AGING_READY        0x0000  //�Զ��ϻ����̲�������׼����
#define STEP_AGING_RUN          0x0001  //�Զ��ϻ�����������
#define STEP_AGING_ERR_SOURCE   0x0002  //���ģ����ϵ���DCC����
#define STEP_AGING_ERR          0x0003  //ģ���ϻ������й���
#define STEP_AGING_OK           0x0004  //ģ���ϻ�������������


//�����嶨��


//���ⲿ�ļ�����ȫ�ֱ�������
extern struct_Auto_Aging    Auto_Aging;
extern struct_Image         Image_Protect_Bak;

extern const unsigned char ubStep250A_Buffer[112];
extern const unsigned char ubStep251A_Buffer[112];
extern const unsigned char ubStep252A_Buffer[112];
extern const unsigned char ubStep253A_Buffer[112];

extern const unsigned char ubStep250B_Buffer[112];
extern const unsigned char ubStep251B_Buffer[112];
extern const unsigned char ubStep252B_Buffer[112];
extern const unsigned char ubStep253B_Buffer[112];

//���ⲿ�ļ����ź�������
extern void WorkStep_Aging_WritetoFlash(void);
extern void Auto_Aging_Init(void);
extern void Auto_Aging_Run(void);

#endif /* APP_INCLUDE_AUTO_AGING_H_ */
