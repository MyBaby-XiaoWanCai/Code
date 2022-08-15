/*
 * DCDC_BSP.h
 *
 *  Created on: 2021年6月20日
 *      Author: JC
 */

#ifndef DSP28377D_BSP_INCLUDE_DCDC_BSP_H_
#define DSP28377D_BSP_INCLUDE_DCDC_BSP_H_
#include "PI_CLA.h"
/*
 * DCDC_BSP.h
 *
 *  Created on: 2021年6月19日
 *      Author: JC
 */
#define FAULT_HARD         0x00000001
#define FAULT_BUS_H        0x00000002
#define FAULT_BUS_L        0x00000004
#define FAULT_PreCharg     0x00000008

#define RDADC_DAT      (volatile Uint32 *)0x380000
//DCDC相关常量定义
#define PWM_DEADREDTIME        30              //桥臂上下管死区时间160nS=16*10nS
#define PWM_DEADFEDTIME        35              //桥臂上下管死区时间160nS=16*10nS
#define PWM_PERIOD          1000            //开关周期20uS
#define PWM_PERIOD_AGE      1600            //老化模式开关周期16uS
#define PWM_MAX_CV          1212            //CV模式允许的最大占空比
#define PWM_MAX_DCV         1480            //DCV模式允许的最大占空比
#define PWM_MAX_CHARGE      950          //充电模式最大占空比限制,7.5V/13.2V*(2000-15)=1150
#define PWM_MAX_DISCH       1930            //放电模式最大占空比限制,(1-1.7/14.2)*(2000-15)=1747
#define CUR_SOFTOFF_END     1000            //软关断结束给定电流100mA，小于该值直接关PWM
//PI控制相关参数
#define DCDC_STRUCT_INIT        0       //DCDC状态机数据结构初始化状态
#define DCDC_SHUTDOWN           1       //PWM和输出MOS全部处于关闭状态，故障保护时进入此状态
#define DCDC_STANDBY            2       //待机状态，PWM开环输出极小占空比使输出MOS处于反偏状态
#define DCDC_READY              3       //由待机模式转其它模式时的中间准备状态
#define DCDC_CC                 4       //恒流充电模式
#define DCDC_CCtoCV             5       //恒流转恒压充电模式
#define DCDC_DCC                6       //恒流放电模式
#define DCDC_DCCtoDCV           7       //恒流转恒压放电模式
#define DCDC_STATIC             8       //静置模式，PWM开环输出极小占空比使输出MOS处于反偏状态
#define DCDC_CV                 9       //恒压输出模式，输出电压反馈直接控制PWM，需要较快的反馈环
#define DCDC_DCV                10      //恒压输入模式，输出电压反馈直接控制PWM，需要较慢的反馈环
#define DCDC_RUN                11       //恒流充电模式
#define DCDC_DEBUG              12      //调试模式，调试模块直接设置PWM值
#define DCDC_Waitime            13      //诊断模式1，A通道工作于CV模式，B通道工作与DCC模式
#define DCDC_Waitime1           14      //诊断模式2，B通道工作于CV模式，A通道工作与DCC模式
#define DCDC_Waitime2           15      //充电模式软关断阶段，完成后跳DCDC_PWM_OFF状态
#define DCDC_SOFTOFF            16      //放电模式软关断阶段，完成后跳DCDC_PWM_OFF状态
#define DCDC_PWM_OFF            17      //直接关闭PWM状态，完成后跳DCDC_SHUTDOWN
#define DCDC_READY1             18      //由待机模式转其它模式时的中间准备状态
#define DCDC_READY2             19      //由待机模式转其它模式时的中间准备状态
#define DCDC_READY3             20      //由待机模式转其它模式时的中间准备状态

#define CHANNELS 4
#define ISR_FREQUENCY   10
#define PI           3.14


#define VOL_BUS_H           94          //Bus电压过高保护
#define VOL_BUS_L           70          //Bus电压过低保护
/*typedef struct struct_PI
{
   Uint16 Cnt;                          //PI运算调用周期计数器
   Uint16 Period;                       //PI运算调用周期
   float32 Ref;                          //参考值
   float32 Feed;                         //反馈值
   float32 Error;                        //偏差
   float32 Limit_Err_Max;                //正向偏差限制
   float32 Limit_Err_Min;                //负向偏差限制
   float32 Kp;                           //比例参数
   float32 Ki;                           //积分参数
   float32 Out_P;                        //比例输出
   float32 Out_I;                        //积分输出
   float32 Out_PI;                       //PI运算输出
   float32 Limit_PI_max;                 //I输出上限
   float32 Limit_PI_min;                 //I输出下限
   float32 Limit_Out_max;                //Out输出上限
   float32 Limit_Out_min;                //Out输出下限
   float32 Out;                          //缩小限幅后的输出
} struct_PI;*/

typedef struct INTERFACE_DCDC
{
    //设置数据，工作状态中都可以起作用
    float Req_ILSet1;               //请求恒流输出时的电流
    float Req_ILSet2;               //请求恒流输出时的电流
    float Req_Ioutsense;
    float Req_VoutSense;
    //设置数据，只在模式切换时起作用
     Uint16 Req_Mode;                    //请求运行的工步模式，具体见DCDC_STRUCT_INIT等定义
     Uint16 Req_ChangeMode;              //DCDC工作模式改变，请求模式切换
     int32 Req_Cur_Total;               //请求恒流输出时的电流,0.1mA/LSB
     int32 Req_Vol;                     //请求输出的电压,0.1mV/LSB
     Uint16 Req_Refresh_Cur;             //请求刷新电流给定值

} INTERFACE_DCDC;
typedef struct struct_Channel
{
     int32 Set_Cur;                     //当前通道目标电流
    Uint16 EN_SynRect;                  //同步整流使能标志
    Uint16 PWMxA_Debug;                 //调试模式用,用来直接设定PWMxA的占空比
    Uint16 PWMxB_Debug;                 //调试模式用,用来直接设定PWMxB的占空比
    Uint16 Slew_Cur_ON;                 //缓启动电流上升斜率
    Uint16 Slew_Cur_OFF;                //缓关闭电流下降斜


    Uint16 Cnt_FastStart;               //快速启动超时计数器
    //模块内部变量，外部程序不要操作
    Uint16 Cnt_SynRect;                 //同步整流启动延时计数器
    Uint16 Flag_SynRect;                //同步整流已启动标志
    Uint16 Cnt_SynRect_Off;             //同步整流关闭延时计数
    Uint16 PWMxA;                       //当前电流通道PWMxA的占空比
    Uint16 PWMxAHR;
    Uint16 PWMxB;                       //当前电流通道PWMxB的占空比
    Uint16 PWMxBHR;
     int32 Now_Cur;                     //当前通道定标转换后的电流值
    Uint16 Cnt_Out_OC;
    Uint16 Flag_MOS_Out;                //输出MOS开关状态标志

    Uint16 Req_FastStart;             //快速启动请求
    Uint16 Req_FastStart_DCM;              //快速启动请求

} struct_Channel;
typedef struct struct_DCDC_SERVICE
{
     float Set_ILSet1;
     float Set_ILSet2;
     float Set_Iout;
     float Ref_Iout;
     float CV_Ref_Iout;
     float Ref_Iout_Min;
     float Ref_Vout;
     float Set_Vout;
    //模块内部使用变量
     Uint16 Now_Mode;                    //DCDC控制状态机当前状态，
     Uint16 Run_Mode;
     Uint16 Flag_Mode;
     float32 Now_Vol_Out;                 //模块输出端电压
     float32 Now_Vol_Bus;                 //模块输入Bus电压
     float32 Now_Vol_JR;
     float32 Now_Vol_Sense;
     float32 PWM_Ready;
     float32 Slew_Cur_ON;
     float32 Slew_Cur_OFF;
     Uint16 Cnt_Delay;
     Uint32 Fault_Module;
     Uint16 Type;
     Uint16 Flag_Swtich;
     Uint16 Flag_Once;
     Uint16 CV_Cnt;
     Uint16 CV_Period;
} struct_DCDC_SERVICE;
typedef struct struct_CH_PUBLIC_VARIABLE
{
    float32 Slew_Cur_ON;                 //缓启动电流上升斜率
    float32 Slew_Cur_OFF;                //缓关闭电流下降斜率
    float32 Limit_Set_Cur;
    Uint16 Parall_Total;

    Uint16 Flag_Cur;
    Uint16 Flag_Record;
}struct_CH_PUBLIC_VARIABLE;

typedef struct struct_FILTER_COEFF
{
   float  FILTER_CC2;
   float  FILTER_CC1;
   float  LPF500;
   float  ts0;
   float  sampletime;
   Uint16  samplefreq;
   float  filter_u2[4][7];
   float filter_u1[4][7];
   float filter_i2[4][7];
   float filter_i1[4][7];
   float Ad_Result[4][7];
}struct_FILTER_COEFF;

typedef struct struct_Data_AD_Sum
{
    Uint16 Cnt_Sum;         //当前累加次数计数器
    Uint16 Num_Avg;         //平均值累加次数

     int32 Sum_Vol_Sense_AD;   //累加过程累加值
     int32 Sum_Vol_Out_AD;
     int32 Sum_Vol_Bus_AD;
     int32 Sum_Cur_CHA_AD;
     int32 Sum_Cur_CHB_AD;
     int32 Sum_Cur_Out_AD;
     int32 Sum_Cur_Out_AD_Copy;
     int32 Sum_Vol_Sense_AD_Copy;//累加Num_Avg次后的累加值
     int32 Sum_Vol_Out_AD_Copy;
     int32 Sum_Vol_Bus_AD_Copy;
     int32 Sum_Cur_CHA_AD_Copy;
     int32 Sum_Cur_CHB_AD_Copy;
} struct_Data_AD_Sum;

typedef struct struct_Data_Sum
{
    Uint16 Cnt_Sum;         //当前累加次数计数器
    Uint16 Num_Avg;         //平均值累加次数
    Uint16 Flag_Refresh;    //避免外部循环使用数据期间数据被中断修改



    float32 Vol_Sense;
    float32 Cur_Out;
    float32 Vol_Out;
    float32 Vol_Bus;
     float32 Vol_JR;     //吉尔电压
     int32 Cur_CHA;
     int32 Cur_CHB;
     int32 Vol_5V;
     int32 AD_NTC_BATB;

     float32 Sum_Vol_Sense;   //累加过程累加值
     float32 Sum_Cur_Out;   //累加过程累加值
     float32 Sum_Vol_Out;
     float32 Sum_Vol_Bus;
     int32 Sum_Cur_CHA;
     int32 Sum_Cur_CHB;
     float32 Sum_Vol_JR;
     int32 Sum_AD_NTC1;
     int32 Sum_AD_NTC2;
     int32 Sum_AD_NTC_BATB;

     float32 Sum_Vol_Sense_Copy;//累加Num_Avg次后的累加值
     float32 Sum_Cur_Out_Copy;//累加Num_Avg次后的累加值
     float32 Sum_Vol_Out_Copy;
     float32 Sum_Vol_Bus_Copy;
     int32 Sum_Cur_CHA_Copy;
     int32 Sum_Cur_CHB_Copy;
     float32 Sum_Vol_JR_Copy;
     int32 Sum_AD_NTC1_Copy;
     int32 Sum_AD_NTC2_Copy;
     int32 Sum_AD_NTC_BATB_Copy;
} struct_Data_Sum;

extern volatile struct EPWM_REGS *ePWM[9];

extern struct_DCDC_SERVICE DCDC_Service[CHANNELS];       //DCDC状态机相关数据
extern INTERFACE_DCDC Interface_DCDC[CHANNELS];          //DCDC模块工作参数请求接口
extern struct_Data_Sum Data_Sum[CHANNELS];
extern void DCDC_Service_BSP(void);
extern void InitDCDC_BSP(void);
extern struct_Data_AD_Sum Data_AD_Sum[CHANNELS];
extern struct_Channel CH_A[CHANNELS];
extern struct_Channel CH_B[CHANNELS];
//extern PI_CLA CH_A_PI_Cur[CHANNELS];
//extern PI_CLA CH_B_PI_Cur[CHANNELS];
extern PI_CLA CH_PI_Iout[CHANNELS];
extern PI_CLA CH_PI_Vout[CHANNELS];
extern PI_CLA CH_PI_Vsense[CHANNELS];
extern Uint32 gCputime0_time3,gCputime0_max;
extern struct_CH_PUBLIC_VARIABLE  CH_Public_Variable;
extern void SynRect_Buck(struct_Channel *Ptr);
extern void SynRect_Boost(struct_Channel *Ptr);
#endif /* DSP28377D_BSP_INCLUDE_DCDC_BSP_H_ */
