/*
 * DCDC_BSP.h
 *
 *  Created on: 2021年6月20日
 *      Author: JC
 */

#ifndef DSP28377D_BSP_INCLUDE_DCDC_BSP_H_
#define DSP28377D_BSP_INCLUDE_DCDC_BSP_H_

/*
 * DCDC_BSP.h
 *
 *  Created on: 2021年6月19日
 *      Author: JC
 */

//DCDC相关常量定义
#define PWM_DEADTIME        23              //桥臂上下管死区时间160nS=16*10nS
#define PWM_PERIOD          2000            //开关周期20uS
#define PWM_PERIOD_AGE      1600            //老化模式开关周期16uS
#define PWM_MAX_CV          1212            //CV模式允许的最大占空比
#define PWM_MAX_DCV         1480            //DCV模式允许的最大占空比
#define PWM_MAX_CHARGE      1150            //充电模式最大占空比限制,7.5V/13.2V*(2000-15)=1150
#define PWM_MAX_DISCH       1930            //放电模式最大占空比限制,(1-1.7/14.2)*(2000-15)=1747
#define CUR_SOFTOFF_END     1000            //软关断结束给定电流100mA，小于该值直接关PWM
//PI控制相关参数
#define DCDC_STRUCT_INIT        0       //DCDC状态机数据结构初始化状态
#define DCDC_SHUTDOWN           1       //PWM和输出MOS全部处于关闭状态，故障保护时进入此状态
#define DCDC_STANDBY            2       //待机状态，PWM开环输出极小占空比使输出MOS处于反偏状态
//#define DCDC_READY              3       //由待机模式转其它模式时的中间准备状态N
#define DCDC_READY1             18      //由待机模式转其它模式时的中间准备状态
#define DCDC_READY2             19      //由待机模式转其它模式时的中间准备状态
#define DCDC_DEBUG              3      //调试模式，调试模块直接设置PWM值
#define DCDC_SOFTOFF_CH         15      //充电模式软关断阶段，完成后跳DCDC_PWM_OFF状态
#define DCDC_SOFTOFF_DISCH      16      //放电模式软关断阶段，完成后跳DCDC_PWM_OFF状态
#define DCDC_PWM_OFF            17      //直接关闭PWM状态，完成后跳DCDC_SHUTDOWN
#define CHANNELS 4

typedef struct struct_PI
{
   Uint16 Cnt;                          //PI运算调用周期计数器
   Uint16 Period;                       //PI运算调用周期
    int32 Ref;                          //参考值
    int32 Feed;                         //反馈值
    int32 Error;                        //偏差
    int32 Limit_Err_Max;                //正向偏差限制
    int32 Limit_Err_Min;                //负向偏差限制
   Uint32 Kp;                           //比例参数
   Uint32 Ki;                           //积分参数
    int32 Out_P;                        //比例输出
    int32 Out_I;                        //积分输出
    int32 Out_PI;                       //PI运算输出
    int32 Limit_PI_max;                 //I输出上限
    int32 Limit_PI_min;                 //I输出下限
    int16 Q_Type;                       //Out_PI到Out右移位数
    int32 Limit_Out_max;                //Out输出上限
    int32 Limit_Out_min;                //Out输出下限
    int32 Forward_Out;                  //前馈补偿
    int32 Out;                          //缩小限幅后的输出
} struct_PI;

typedef struct INTERFACE_DCDC
{
    //设置数据，工作状态中都可以起作用
    float Req_ILSet1_Pu;               //请求恒流输出时的电流
    float Req_ILSet2_Pu;               //请求恒流输出时的电流
    float Req_Ioutsense_Pu;
    float Req_VoutSense_Pu;
    //设置数据，只在模式切换时起作用
     Uint16 Req_Mode;                    //请求运行的工步模式，具体见DCDC_STRUCT_INIT等定义
     Uint16 Req_ChangeMode;              //DCDC工作模式改变，请求模式切换
} INTERFACE_DCDC;
typedef struct struct_Channel
{
     int32 Set_Cur;                     //当前通道目标电流

    Uint16 EN_Debug;                    //调试模式使能标志
    Uint16 PWMxA_Debug;                 //调试模式用,用来直接设定PWMxA的占空比
    Uint16 PWMxB_Debug;                 //调试模式用,用来直接设定PWMxB的占空比


    //模块内部变量，外部程序不要操作
    Uint16 Cnt_SynRect;                 //同步整流启动延时计数器
    Uint16 Flag_SynRect;                //同步整流已启动标志
    Uint16 Cnt_SynRect_Off;             //同步整流关闭延时计数
    Uint16 PWMxA;                       //当前电流通道PWMxA的占空比
    Uint16 PWMxB;                       //当前电流通道PWMxB的占空比
     int32 Now_Cur;                     //当前通道定标转换后的电流值
    Uint16 Cnt_Out_OC;
    Uint16 Flag_MOS_Out;                //输出MOS开关状态标志
    struct_PI *Ptr_PI_Cur_Charge;       //充电模式电流环PI控制结构体指针，在结构体初始化时指向PI_Cur_Charge
    struct_PI *Ptr_PI_Cur_Discharge;    //放电模式电流环PI控制结构体指针，在结构体初始化时指向PI_Cur_Disharge

} struct_Channel;
typedef struct struct_DCDC_SERVICE
{
     float Set_ILSet1_Pu;
     float Set_ILSet2_Pu;
     float Set_Ioutsense_Pu;
     float Set_VoutSense_Pu;
    //模块内部使用变量
     Uint16 Now_Mode;                    //DCDC控制状态机当前状态，
     float32 Now_Vol_Out;                 //模块输出端电压
     float32 Now_Vol_Bus;                 //模块输入Bus电压
     float32 PWM_Ready;
     Uint16 Cnt_Delay;
     Uint32 Fault_Module;
} struct_DCDC_SERVICE;
extern volatile struct EPWM_REGS *ePWM[9];
extern struct_DCDC_SERVICE DCDC_Service[CHANNELS];       //DCDC状态机相关数据
extern INTERFACE_DCDC Interface_DCDC[CHANNELS];          //DCDC模块工作参数请求接口
extern void DCDC_Service_BSP(void);
extern void InitDCDC_BSP(void);
extern Uint16 CMPAValue[4],CMPBValue[4];
#endif /* DSP28377D_BSP_INCLUDE_DCDC_BSP_H_ */
