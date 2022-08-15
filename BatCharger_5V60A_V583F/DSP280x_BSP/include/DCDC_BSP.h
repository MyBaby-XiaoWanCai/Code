/**************************************************************************************/
// Copyright (C) Wuhan Jingneng Electronic Technology Co., Ltd. All Rights Reserved
//    Project: BatCharger_5V60A_V200
//  File name: DCDC_BSP.h
// Created on: 2020-3-2
//     Author: LuoAnJiang
/**************************************************************************************/

#ifndef DSP2803X_BSP_INCLUDE_DCDC_BSP_H_
#define DSP2803X_BSP_INCLUDE_DCDC_BSP_H_
//DCDC相关常量定义
#define PWM_DEADTIME        16              //桥臂上下管死区时间160nS=16*10nS
#define PWM_PERIOD          2000            //开关周期20uS
#define PWM_PERIOD_AGE      1600            //老化模式开关周期16uS
#define PWM_MAX_CV          1212            //CV模式允许的最大占空比
#define PWM_MAX_DCV         1480            //DCV模式允许的最大占空比
#define PWM_MAX_CHARGE      1150            //充电模式最大占空比限制,7.5V/13.2V*(2000-15)=1150
#define PWM_MAX_DISCH       1930            //放电模式最大占空比限制,(1-1.7/14.2)*(2000-15)=1747
#define CUR_SOFTOFF_END     1000            //软关断结束给定电流100mA，小于该值直接关PWM

#if   VER_PRODUCT == PRODUCT_5V20A
    #define CUR_TO_SINGLE_CH    50000L      //单机或并机模式，每通道平均电流小于该值，切换到单通道模式
    #define CUR_TO_LOW_RANGE    100000      //切换到低量程电流档位门限,单个通道
#elif VER_PRODUCT == PRODUCT_5V30A
    #define CUR_TO_SINGLE_CH    20000L      //单机或并机模式，每通道平均电流小于该值，切换到单通道模式
    #define CUR_TO_LOW_RANGE    80000       //切换到低量程电流档位门限,单个通道
#elif VER_PRODUCT == PRODUCT_5V60A
    #define CUR_TO_SINGLE_CH    20000L      //单机或并机模式，每通道平均电流小于该值，切换到单通道模式
    #define CUR_TO_LOW_RANGE    80000       //切换到低量程电流档位门限,单个通道
#elif VER_PRODUCT == PRODUCT_5V80A
    #define CUR_TO_SINGLE_CH    20000L      //单机或并机模式，每通道平均电流小于该值，切换到单通道模式
    #define CUR_TO_LOW_RANGE    80000       //切换到低量程电流档位门限,单个通道
#endif

#define CUR_FAST_OC_H       28900           //快速过流保护AD绝对值门槛，约50A/CH
#define CUR_FAST_OC_L       32700           //快速过流保护AD绝对值门槛，约11.3A/CH
#define CUR_SYNRECT_OFF     50000           //同步整流关闭电流
#define VOL_BUS_H           165000          //Bus电压过高保护
#define VOL_BUS_L           115000          //Bus电压过低保护
#define CUR_TO_FAST_START	20000			//
#define CUR_SLEW_MAX        500             //
#define CUR_SLEW_DEFAULT    420             //

//PWM使能接口定义
#define PWM1ON     {EALLOW; EPwm1Regs.TZSEL.all=0x0000; EPwm1Regs.TZCLR.all=0x0007;   \
                    EPwm1Regs.TZFRC.all=0x0000; EDIS;}
#define PWM1OFF    {EALLOW; EPwm1Regs.TZSEL.all=0x0100; EPwm1Regs.TZCTL.all=0x000A;   \
                    EPwm1Regs.TZFRC.all=0x0004; EDIS; EPwm1Regs.CMPA.half.CMPA=PWM_DEADTIME; EPwm1Regs.CMPB=PWM_PERIOD;}

#define PWM2ON     {EALLOW; EPwm2Regs.TZSEL.all=0x0000; EPwm2Regs.TZCLR.all=0x0007;   \
                    EPwm2Regs.TZFRC.all=0x0; EDIS;}
#define PWM2OFF    {EALLOW; EPwm2Regs.TZSEL.all=0x0100; EPwm2Regs.TZCTL.all=0x000A;   \
                    EPwm2Regs.TZFRC.all=0x0004; EDIS; EPwm2Regs.CMPA.half.CMPA=PWM_DEADTIME; EPwm2Regs.CMPB=PWM_PERIOD;}

//结构体定义
//从SPI口读取进来的AD值按通道号保存结构体
typedef struct struct_Data_AD
{
     int16 AD7606_CH0;
     int16 AD7606_CH1;
     int16 AD7606_CH2;
     int16 AD7606_CH3;
     int16 AD7606_CH4;
     int16 AD7606_CH5;
     int16 AD7606_CH6;
     int16 AD7606_CH7;
     int16 F2808_CH0;
     int16 F2808_CH1;
     int16 F2808_CH2;
} struct_Data_AD;

//定标转换后的模拟量数据平均值
typedef struct struct_Data_Sum
{
    Uint16 Cnt_Sum;         //当前累加次数计数器
    Uint16 Num_Avg;         //平均值累加次数
    Uint16 Flag_Refresh;    //避免外部循环使用数据期间数据被中断修改

     int16 AD_PT;           //AD值得平均值
     int16 AD_NTC1;
     int16 AD_NTC2;
     Uint16 Temp_PT;         //物理量
     Uint16 Temp_NTC1;
     Uint16 Temp_NTC2;
     Uint16 Temp_Sink_S1;
     Uint16 Temp_Shunt_S1;

     int32 Vol_Sense;
     int32 Vol_Out;
     int32 Vol_Bus;
     int32 Vol_Contact;		//触点电压
     int32 Cur_CHA;
     int32 Cur_CHB;
     int32 Vol_5V;
     int32 AD_NTC_BATB;

     int32 Sum_Vol_Sense;   //累加过程累加值
     int32 Sum_Vol_Out;
     int32 Sum_Vol_Bus;
     int32 Sum_Cur_CHA;
     int32 Sum_Cur_CHB;
     int32 Sum_AD_PT;
     int32 Sum_AD_NTC1;
     int32 Sum_AD_NTC2;
     int32 Sum_AD_NTC_BATB;

     int32 Sum_Vol_Sense_Copy;//累加Num_Avg次后的累加值
     int32 Sum_Vol_Out_Copy;
     int32 Sum_Vol_Bus_Copy;
     int32 Sum_Cur_CHA_Copy;
     int32 Sum_Cur_CHB_Copy;
     int32 Sum_AD_PT_Copy;
     int32 Sum_AD_NTC1_Copy;
     int32 Sum_AD_NTC2_Copy;
     int32 Sum_AD_NTC_BATB_Copy;
} struct_Data_Sum;

//定标转换后的模拟量数据平均值
typedef struct struct_Data_AD_Sum
{
    Uint16 Cnt_Sum;         //当前累加次数计数器
    Uint16 Num_Avg;         //平均值累加次数

     int32 Sum_Vol_Sense_AD;   //累加过程累加值
     int32 Sum_Vol_Out_AD;
     int32 Sum_Vol_Bus_AD;
     int32 Sum_Cur_CHA_AD;
     int32 Sum_Cur_CHB_AD;

     int32 Sum_Vol_Sense_AD_Copy;//累加Num_Avg次后的累加值
     int32 Sum_Vol_Out_AD_Copy;
     int32 Sum_Vol_Bus_AD_Copy;
     int32 Sum_Cur_CHA_AD_Copy;
     int32 Sum_Cur_CHB_AD_Copy;
} struct_Data_AD_Sum;

typedef struct struct_Cur_Scale
{
     int32 K_Now;       //数据定标实际使用K值变量
     int32 B_Now;       //数据定标实际使用B值变量，B_Now=B_ROM*10+B_C_Env+B_C_Cur+B_C_Res
     int32 K_ROM_CH;    //EEPROM读取出来的K值
     int32 B_ROM_CH;    //EEPROM读取出来的B值
     int32 K_ROM_DIS;   //EEPROM读取出来的K值
     int32 B_ROM_DIS;   //EEPROM读取出来的B值
     int32 B_C_Env;     //环境温升补偿B值，
     int32 B_C_Cur;     //电流温升补偿B值，
     int32 B_C_Res;     //老模块输出电阻补偿B值，
} struct_Cur_Scale;

typedef struct struct_Vol_Scale
{
     int32 K_ROM_Out;
     int32 B_ROM_Out;
     int32 K_Now_Out;
     int32 B_Now_Out;

     int32 K_ROM_Sense;
     int32 B_ROM_Sense;
     int32 K_Now_Sense;
     int32 B_Now_Sense;

     int32 K_ROM_Bus;
     int32 K_Now_Bus;
} struct_Vol_Scale;

//PI控制相关参数
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
    int32 Forward_Out;					//前馈补偿
    int32 Out;                          //缩小限幅后的输出
} struct_PI;

//快速保护用，防止电源损坏
typedef struct struct_Prot_Fast
{
    Uint32 Limit_Cur_Out;               //输出电流绝对值快速过流保护门限
    Uint16 Limit_Cur_Out_Num;           //输出电流过流保护需要连续持续的次数
    Uint16 Limit_Cur_Out_Cnt;           //输出电流超限计数器
    Uint32 Limit_Vol_Out;               //输出电压快速过压保护门限
    Uint16 Limit_Vol_Out_Num;           //输出电压过压保护需要连续持续的次数
    Uint16 Limit_Vol_Out_Cnt;           //输出电压超限计数器
    Uint32 Limit_Vol_Bus;               //输入电压快速过压保护门限
    Uint16 Limit_Vol_Bus_Num;           //输入电压过压保护需要连续持续的次数
    Uint16 Limit_Vol_Bus_Cnt;           //输入电压超限计数器
} struct_Prot_Fast;

typedef struct INTERFACE_DCDC
{
    //设置数据，工作状态中都可以起作用
     int32 Req_Cur_Total;               //请求恒流输出时的电流,0.1mA/LSB
     int32 Req_Vol;                     //请求输出的电压,0.1mV/LSB
    Uint16 Req_Refresh_Cur;             //请求刷新电流给定值
    Uint16 Req_Refresh_Vol;             //请求刷新电压给定值
    Uint16 Req_CurRange;               //电流档位,0-高档位，1-低档位

    //设置数据，只在模式切换时起作用
    Uint16 Req_Mode;                    //请求运行的工步模式，具体见DCDC_STRUCT_INIT等定义
    Uint16 Req_ChangeMode;              //DCDC工作模式改变，请求模式切换
    Uint16 Parall_Total;                //并机数量1-单机，2-双机，3-三机，4-四机
    Uint16 EN_Run;                      //模块是否允许运行标志
    Uint16 EN_MOS_Out;                  //输出MOS是否允许开启标志
    Uint16 EN_CHA_Comm;                 //通讯口或其它模块控制CHA通道是否允许运行标志
    Uint16 EN_CHA_SynRect;              //通讯口或其它模块控制CHA通道是否允许同步整流标志
    Uint16 EN_CHB_Comm;                 //通讯口或其它模块控制CHB通道是否允许运行标志
    Uint16 EN_CHB_SynRect;              //通讯口或其它模块控制CHB通道是否允许同步整流标志
    Uint16 Mode_Parall;                 //模块并机或单机工作模式，0-单机模式，1-并机主机，2-并机从机
    Uint16 Req_Aging;                   //老化模式标志
    Uint16 EN_MOS_Out_Slaver;           //并机主机模式，通讯口或其它模块控制从机输出MOS是否开启
    Uint16 EN_Run_Slaver;               //并机主机模式，通讯口或其它模块控制从机是否允许运行
} INTERFACE_DCDC;

//Req_Mode,DCDC_Service状态机状态定义
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
#define DCDC_DIAGN_A2B          12      //诊断模式1，A通道工作于CV模式，B通道工作与DCC模式
#define DCDC_DIAGN_B2A          13      //诊断模式2，B通道工作于CV模式，A通道工作与DCC模式
#define DCDC_DEBUG              14      //调试模式，调试模块直接设置PWM值
#define DCDC_SOFTOFF_CH         15      //充电模式软关断阶段，完成后跳DCDC_PWM_OFF状态
#define DCDC_SOFTOFF_DISCH      16      //放电模式软关断阶段，完成后跳DCDC_PWM_OFF状态
#define DCDC_PWM_OFF            17      //直接关闭PWM状态，完成后跳DCDC_SHUTDOWN
#define DCDC_READY1             18      //由待机模式转其它模式时的中间准备状态
#define DCDC_READY2             19      //由待机模式转其它模式时的中间准备状态

//Mode_Parall并机模式定义
#define SINGLE                  0
#define PARALL_MASTER           1
#define PARALL_SLAVER           2

typedef struct struct_DCDC_SERVICE
{
     int32 Set_Cur_Total;               //目标电流,在模式转换时从接口的Req_Cur_Total拷贝过来
     int32 Set_Vol;                     //目标电压,在模式转换时从接口的Req_Vol拷贝过来
    Uint16 Parall_Total;                //并机数量,在模式转换时从接口的Num_Parral拷贝过来
    Uint16 Mode_Parall;                 //模块并机或单机工作模式，0-单机模式，1-并机主机，2-并机从机
    Uint16 Flag_Aging;                  //老化模式使能标志

    //模块内部使用变量
    Uint16 Now_Mode;                    //DCDC控制状态机当前状态，
     int32 Now_Cur_AB;                  //单机A、B通道电流平均值的和
     int32 Now_Vol_Sense;               //当前Sense端电压
     int32 Now_Vol_Out;                 //模块输出端电压
     int32 Now_Vol_Bus;                 //模块输入Bus电压
    // int16 T_Sink1;                     //散热器1温度,对应JP?插座
    Uint16 PWM_Ready;                   //Ready1状态PWM需要开启的脉宽,用于预充输出电容电压
    Uint16 PWM_DCM;						//DCM模式，根据目标电流，计算需要达到的PWM脉宽
    Uint16 Cnt_Delay;                   //状态机内部延时用
    Uint16 Cnt_CCtoCV;                  //CC转CV延时计数器
    Uint16 Num_CCtoCV;                  //CC转CV快速检测需要检测的次数
    Uint16 Cnt_CCtoCV2;                  //CC转CV延时计数器
    Uint16 Flag_CV_Step;                //进入恒压充电阶段标志
    Uint16 Cnt_DCCtoDCV;                //DCC转DCV延时计数器
    Uint16 Num_DCCtoDCV;
    Uint16 Cnt_DCCtoDCV2;                //DCC转DCV延时计数器
    Uint16 Flag_DCV_Step;               //进入恒压放电阶段标志
     int32 Cur_Fast_OC_AD;              //快速过流保护门槛AD值
    Uint16 Cnt_FastRefresh;             //工步转换后前一秒快速刷新数据
    Uint16 Cnt_Time_CV_DCV;             //进入恒压阶段运行时长

    Uint16 Flag_CV_Step2;                //进入恒压充电阶段标志2
    Uint16 Flag_DCV_Step2;               //进入恒压放电阶段标志2
    Uint16 Flag_SingleCH;               //单通道工作模式标志
    Uint16 Flag_CurRange;               //电流档位,0-高档位，1-低档位
    Uint16 Flag_Soft_OFF;               //软关断已经运行标志
     int32 Limit_Set_Vol;               //请求电压上限,上电后根据模块型号初始化一次,0.1mV/LSB
    Uint16 Slew_Vol_ON;                 //CV模式缓启动电压上升斜率
    Uint16 Cnt_Bus_H;
    Uint32 Cnt_Bus_L;
    Uint16 Cnt_Out_H;                   //模块输出端口快速过压保护
    Uint16 Cnt_Fault_AD7606;            //AD7606数据读取错误计数器
    Uint16 Cnt_Timeout_Busy;            //AD7606的Busy信号超时计数器
    Uint32 Fault_Module;                //模块非化成工艺相关故障
    Uint32 Fault_Slaver1;               //模块1非化成工艺相关故障
    Uint32 Fault_Slaver2;               //模块2非化成工艺相关故障
    Uint32 Fault_Slaver3;               //模块3非化成工艺相关故障
    Uint32 Fault_Slaver4;               //模块4非化成工艺相关故障
     int32 Sum_Vol_Sense;               //电压环计算电压平均值用
     int32 Vol_Sense_LPF;               //20mS电压平均值
     int32 Vol_Sense_LPF_Last;
    Uint16 Cnt_Sum_Vol_Sense;           //Sense电压平均用计数器
    Uint16 Num_Sum_Vol_Sense;           //Sense电压平均次数
    Uint32 Time_Running;                //同一工步连续运行时间，20uS/LSB

    struct_PI *Ptr_PI_Vol_Charge;       //CCtoCV模式电压环PI控制数据结构体指针
    struct_PI *Ptr_PI_Vol_Discharge;    //DCCtoDCV模式电压环PI控制数据结构体指针
    struct_PI *Ptr_PI_Vol_CV;           //CV模式电压环PI控制数据结构体指针
    struct_PI *Ptr_PI_Vol_DCV;          //DCV模式电压环PI控制数据结构体指针
    struct_Vol_Scale Vol_Scale;         //Out电压和Sense电压校准数据
} struct_DCDC_SERVICE;

//高低电流档位标志定义,Flag_CurRange
#define CUR_RANGE_H         0           //高电流档位标志定义
#define CUR_RANGE_L         1           //低电流档位标志定义

//模块故障比特定义,Fault_Module
#define FAULT_OC_CHA        0x00000001      //单机模式，并机主机模式，并机从机模式，模块A通道输出口正向或负向过流
#define FAULT_OC_CHB        0x00000002      //单机模式，并机主机模式，并机从机模式，模块B通道输出口正向或负向过流
#define FAULT_FAN           0x00000004      //单机模式，并机主机模式，并机从机模式，模块风扇故障
#define FAULT_OT_SINK       0x00000008      //单机模式，并机主机模式，并机从机模式，模块内部过热保护
#define FAULT_BUS_H         0x00000010      //单机模式，并机主机模式，并机从机模式，模块输入Bus电压过高保护
#define FAULT_BUS_L         0x00000020      //单机模式，并机主机模式，并机从机模式，模块输入Bus电压过低保护，上电后延迟2S开始检测
#define FAULT_DATA_CALI     0x00000040      //单机模式，并机主机模式，并机从机模式，模块校准数据异常
#define FAULT_SLAVER_CAN    0x00000080      //并机从机模式，模块CAN通讯故障
#define FAULT_AD7606        0x00000100      //单机模式，并机主机模式，并机从机模式，模块读取AD7606数据错误
#define FAULT_CH_IMBALA     0x00000200      //单机模式，并机主机模式，并机从机模式，在老化模式的CV工步单通道电流超范围
//#define FAULT_FLASH         0x00000400      //单机模式，并机主机模式，并机从机模式，模块外挂FLASH读写错误
#define FAULT_OUT_VOL_HIGH  0x00000400      //单机模式，并机主机模式，并机从机模式，模块外挂FLASH读写错误
#define FAULT_OUTMOS        0x00000800      //单机模式，并机主机模式，并机从机模式，模块输出控制MOS故障
#define FAULT_ANALOG_CHA    0x00001000      //单机模式，并机主机模式，并机从机模式，模块A通道电流采样模拟电路异常
#define FAULT_ANALOG_CHB    0x00002000      //单机模式，并机主机模式，并机从机模式，模块B通道电流采样模拟电路异常
#define FAULT_ANALOG_PT     0x00004000      //单机模式，并机主机模式，模块电池温度采样模拟电路异常
#define FAULT_ANALOG_NTC    0x00008000      //单机模式，并机主机模式，并机从机模式，模块散热器温度采样模拟电路异常

#define FAULT_S1_CAN        0x01000000      //并机主机，从机1CAN通讯故障标志
#define FAULT_S2_CAN        0x02000000      //并机主机，从机2CAN通讯故障标志
#define FAULT_S3_CAN        0x04000000      //并机主机，从机3CAN通讯故障标志
#define FAULT_S4_CAN        0x08000000      //并机主机，从机4CAN通讯故障标志
#define FAULT_SLAVER1       0x10000000      //并机主机，从机1故障标志
#define FAULT_SLAVER2       0x20000000      //并机主机，从机2故障标志
#define FAULT_SLAVER3       0x40000000      //并机主机，从机3故障标志
#define FAULT_SLAVER4       0x80000000      //并机主机，从机4故障标志

typedef struct struct_Channel
{
     int32 Set_Cur;                     //当前通道目标电流
    Uint16 EN_Run;                      //模块内部设定通道工作使能标志
    Uint16 EN_Run_Comm;                 //外部通讯口设定通道工作使能标志
    Uint16 EN_SynRect;                  //同步整流使能标志
    Uint16 EN_Debug;                    //调试模式使能标志
    Uint16 EN_Forward;					//电流环前馈使能控制

    Uint16 PWMxA_Debug;                 //调试模式用,用来直接设定PWMxA的占空比
    Uint16 PWMxB_Debug;                 //调试模式用,用来直接设定PWMxB的占空比
    Uint16 Slew_Cur_ON;                 //缓启动电流上升斜率
    Uint16 Slew_Cur_OFF;                //缓关闭电流下降斜率
     int32 Limit_Set_Cur;               //单通道请求电流上限
    Uint16 Req_FastStart;				//快速启动请求
    Uint16 Req_FastStart_DCM;              //快速启动请求
    Uint16 Cnt_FastStart;				//快速启动超时计数器
    //Uint16 Num_FastStart;				//快速启动累加次数
    //Uint16 ACC_FastStart;				//快速启动累加值

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
    struct_Cur_Scale Cur_Scale_H;       //当前通道电流高档位校准及补偿相关数据
    struct_Cur_Scale Cur_Scale_L;       //当前通道电流低档位校准及补偿相关数据
} struct_Channel;

typedef struct struct_Bus_Forward
{
	Uint16 Cnt_Sample;
	 int32 Vol_Bus_Slow;
	 int32 Vol_Bus_Slow_Last;
	 int32 Vol_Bus_Fast;
	 int32 Vol_Bus_Fast_Last;
	 int32 Vol_Bus_Var;					//Bus电压变化量
	 int32 K_Forward;					//由前馈强度系数
	 int32 K_Forward_Charge;            //充电状态前馈强度系数
     int32 K_Forward_Discharge;         //放电状态前馈强度系数
	 int32 Limit_Pos;					//正向限幅
	 int32 Limit_Neg;					//负向限幅
	 int32 PWM_Forward;					//前馈值
} struct_Bus_Forward;

typedef struct struct_Bus_Fluct_Check
{
    Uint16 Flag_Bus_Fluct;
    Uint16 Flag_Bus_Low;
    Uint16 Cnt_Delay;
    Uint16 Cnt_Mask;
    Uint16 Num_Mask;
} struct_Bus_Fluct_Check;

typedef struct struct_Adjust_PI_Vol
{
    Uint16 Cnt_Rise;
    Uint16 Cnt_Fall;
    Uint16 Num;
     int32 Ref_Vol_Error;
     int32 Limit_Max;
     int32 Limit_Min;
} struct_Adjust_PI_Vol;



//对外部文件开放全局变量声明
extern Uint32 Flag_Tasks_1mS;
extern Uint32 Flag_Tasks_20mS;
extern Uint32 Flag_Tasks_500mS;
extern struct_Data_Sum Data_Sum;           //定标转换后的物理量进行平均后得到的值
extern struct_DCDC_SERVICE DCDC_Service;
extern INTERFACE_DCDC Interface_DCDC;
extern struct_Channel CH_A;               //CHA相关数据
extern struct_Channel CH_B;               //CHB相关数据
extern struct_Data_AD_Sum Data_AD_Sum;
extern struct_PI PI_Vol_Charge;                //CCtoCV模式电压环PI控制数据结构体
extern struct_PI PI_Vol_Discharge;             //DCCtoDCV模式电压环PI控制数据结构体
extern struct_Data_AD Data_AD;            //从AD7606和F2808读出的AD原始数据
extern struct_Bus_Forward Bus_Forward;
//对外部文件开放函数声明
extern interrupt void EPWM1_INT_ISR_BSP(void);
extern void InitDCDC_BSP(void);
extern void InitSPIB_BSP(void);
extern void Function_DCDC_CCtoCV(void);
extern void Function_DCDC_DCCtoDCV(void);
extern void Auto_Adjust_PI_Vol_Charge(void);
extern void Auto_Adjust_PI_Vol_Discharge(void);

#endif /* DSP2803X_BSP_INCLUDE_DCDC_BSP_H_ */
