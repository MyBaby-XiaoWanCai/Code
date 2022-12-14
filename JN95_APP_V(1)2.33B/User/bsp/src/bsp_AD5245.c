/*
*********************************************************************************************************
*
*   模块名称 : AD5245驱动模块(256位可编程电位器)
*   文件名称 : bsp_AD5245.c
*   版    本 : V1.0
*   说    明 : AD5245模块和CPU之间采用IIC接口。本驱动程序使用模拟IIC。
*
*
*   修改记录 :
*       版本号  日期         作者     说明
*       V1.0    2021-08-09   WL      正式发布
*********************************************************************************************************
*/
#include "main.h"
#include "bsp_dwt.h"
#include "bsp_AD5245.h"
extern uint32_t IP_Alarm_Flag[8];       //内部保护标志位
/*
*********************************************************************************************************
*   函 数 名: AD5245_Init
*   功能说明: 初始化芯片IO接口
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void AD5245_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOH_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOH, AD5245_SDA_Pin | AD5245_SCL_Pin, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = AD5245_SDA_Pin | AD5245_SCL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
}
/*
*********************************************************************************************************
*   函 数 名: IIC_Start
*   功能说明: CPU发起I2C总线启动信号
*   形    参:  无
*   返 回 值: 无
*********************************************************************************************************
*/
static void IIC_Start(void)
{
    SDA_OUT;      //sda线输出
    IIC_SDA(1);
    IIC_SCL(1);
    bsp_DelayUS(5);
    IIC_SDA(0);   //START:when CLK is high,DATA change form high to low
    bsp_DelayUS(6);
    IIC_SCL(0);   //钳住I2C总线，准备发送或接收数据
}

/*
*********************************************************************************************************
*   函 数 名: IIC_Stop
*   功能说明: CPU发起I2C总线停止信号
*   形    参:  无
*   返 回 值: 无
*********************************************************************************************************
*/
static void IIC_Stop(void)
{
    SDA_OUT;      //sda线输出
    IIC_SCL(0);
    IIC_SDA(0);
    IIC_SCL(1);
    bsp_DelayUS(6);
    IIC_SDA(1);
    bsp_DelayUS(6);
}

/*
*********************************************************************************************************
*   函 数 名: IIC_Wait_Ack
*   功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*   形    参:  无
*   返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
static uint8_t IIC_Wait_Ack(void)
{
    uint8_t tempTime = 0;
    SDA_IN;
    IIC_SDA(1);
    bsp_DelayUS(1);
    IIC_SCL(1);
    bsp_DelayUS(1);
    while(READ_SDA)
    {
        tempTime++;
        if(tempTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL(0);
    return 0;
}

/*
*********************************************************************************************************
*   函 数 名: IIC_Ack
*   功能说明: CPU产生一个ACK信号
*   形    参:  无
*   返 回 值: 无
*********************************************************************************************************
*/
static void IIC_Ack(void)
{
    IIC_SCL(0);
    SDA_OUT;
    IIC_SDA(0);
    bsp_DelayUS(2);
    IIC_SCL(1);
    bsp_DelayUS(5);
    IIC_SCL(0);
}

/*
*********************************************************************************************************
*   函 数 名: IIC_NAck
*   功能说明: CPU产生1个NACK信号
*   形    参:  无
*   返 回 值: 无
*********************************************************************************************************
*/
static void IIC_NAck(void)
{
    IIC_SCL(0);
    SDA_OUT;
    IIC_SDA(1);
    bsp_DelayUS(2);
    IIC_SCL(1);
    bsp_DelayUS(5);
    IIC_SCL(0);
}

/*
*********************************************************************************************************
*   函 数 名: IIC_Send_Byte
*   功能说明: CPU向I2C总线设备发送8bit数据
*   形    参:  txd ： 等待发送的字节
*   返 回 值: 无
*********************************************************************************************************
*/
static void  IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    SDA_OUT;
    IIC_SCL(0);           //拉低时钟开始数据传输
    for(t = 0; t < 8; t++)
    {
        if((txd & 0x80) > 0) //0x80 1000 0000
        {
            IIC_SDA(1);
        }
        else
        {
            IIC_SDA(0);
        }
        txd <<= 1;
        bsp_DelayUS(2);      //对TEA5767这三个延时都是必须的
        IIC_SCL(1);
        bsp_DelayUS(2);
        IIC_SCL(0);
        bsp_DelayUS(2);
    }
}

/*
*********************************************************************************************************
*   函 数 名: IIC_Read_Byte
*   功能说明: CPU从I2C总线设备读取8bit数据
*   形    参:  ack=1时，发送ACK，ack=0，发送nACK
*   返 回 值: 读到的数据
*********************************************************************************************************
*/
static uint8_t IIC_Read_Byte(uint8_t ack)
{
    unsigned char i, receive = 0;
    SDA_IN;
    for(i = 0; i < 8; i++)
    {
        IIC_SCL(0);
        bsp_DelayUS(2);
        IIC_SCL(1);
        receive <<= 1;
        if(READ_SDA)
        {
            receive++;
        }
        bsp_DelayUS(1);
    }
    if(!ack)
    {
        IIC_NAck();    //发送nACK
    }
    else
    {
        IIC_Ack();    //发送ACK
    }
    return receive;
}

/*
*********************************************************************************************************
*   函 数 名: AD5245_Value_Set
*   功能说明: 数字电位计值设置
*   形    参: addr: 参考写从机地址字节点定义
              value: 数字电位计设置的寄存器值（0~255）
*   返 回 值: 1:操作成功，0：操作失败
*********************************************************************************************************
*/
int AD5245_Value_Set(uint8_t addr, uint8_t value)
{
    IIC_Start();
    IIC_Send_Byte(addr);                 //发送从机地址
    IIC_Wait_Ack();
    IIC_Send_Byte(Normal_SET);           //普通设置指令
    IIC_Wait_Ack();
    IIC_Send_Byte(value);                //发送设置值
    IIC_Wait_Ack();
    IIC_Stop();                          //产生一个停止位
    bsp_DelayMS(5);
    if(AD5245_Value_Read(addr + 1) == value)
    {
        return 1;
    }
    else
    {
                  for(uint8_t j=0;j<CHANNEL_CFG;j++)
                IP_Alarm_Flag[j]|=IntelProtect_PME;    //电位器故障
        return 0;
    }
}

/*
*********************************************************************************************************
*   函 数 名: AD5245_Vpro_Set
*   功能说明: 通过设置数字电位计实现保护基准 2.5*R/100k*2.2 = Vpro,1.977~4.35V
*   数字电位计 0~50K，128~100K  R = 0.3906*value + 50;
*   形    参: Provoltage: 电压保护值

*   返 回 值: 1:操作成功，0：操作失败
*********************************************************************************************************
*/
int AD5245_Vpro_Set(float Vpro)
{
    uint8_t value;
    float AD5425_Set;
    int result;
	
#ifdef YCJ
    AD5425_Set =  (Vpro) * (float)25.5;                //J版硬件：AD5245_Set/255=Rwb/100K, vpro*1000=Rwb/10,YCJ全部按J版硬件修改
#else
////    AD5425_Set = (float)10.9345 * Vpro * (float)2.55;	 //H版硬件：AD5245_Set/255=Rwb/100K, vpro*1000=Rwb*(1573/17200)，4通道机器有部分按J版硬件修改，需要产品溯源
	
	AD5425_Set =  (Vpro) * (float)25.5;                //20220506新增,厦门现场全部使用J版本硬件
#endif
	
    if(AD5425_Set > 255)
    {
        value = 255;
    }
    else if(AD5425_Set < 0)
    {
        value = 0;
    }
    else
    {
        value = (uint8_t)AD5425_Set;
    }
    result = AD5245_Value_Set(Write_A0_Add,value);
    return result;
}

/*
*********************************************************************************************************
*   函 数 名: AD5245_Value_Read
*   功能说明: 数字电位计值读取
*   形    参: addr: 参考写从机地址字节点定义
*   返 回 值: 读出数据值
*********************************************************************************************************
*/
uint8_t AD5245_Value_Read(uint8_t addr)
{
    uint8_t temp = 0;
    IIC_Start();                                    //发送起始位
    IIC_Send_Byte(addr);                            //发送从机地址
    IIC_Wait_Ack();
    temp = IIC_Read_Byte(0);
    IIC_Stop();                                     //产生一个停止位
    return temp;
}


/*
*********************************************************************************************************
*   函 数 名: AD5245_Full_RS
*   功能说明: 满量程复位
*   形    参: addr: 参考写从机地址字节点定义
*   返 回 值: 无
*********************************************************************************************************
*/
void AD5245_Full_RS(uint8_t addr)
{
    IIC_Start();
    IIC_Send_Byte(addr);                  //发送从机地址
    IIC_Wait_Ack();
    IIC_Send_Byte(Normal_SET);            //普通设置指令
    IIC_Wait_Ack();
    IIC_Send_Byte(0xff);                 //发送设置值255
    IIC_Wait_Ack();
    IIC_Stop();                          //产生一个停止位
    bsp_DelayMS(5);
}


/*
*********************************************************************************************************
*   函 数 名: AD5245_Half_RS
*   功能说明: 满量程复位
*   形    参: addr: 参考写从机地址字节点定义
*   返 回 值: 无
*********************************************************************************************************
*/
void AD5245_Half_RS(uint8_t addr)
{
    IIC_Start();
    IIC_Send_Byte(addr);                  //发送从机地址
    IIC_Wait_Ack();
    IIC_Send_Byte(RS_SET);                //复位指令
    IIC_Wait_Ack();
    IIC_Send_Byte(0);                     //复位值128
    IIC_Wait_Ack();
    IIC_Stop();                           //产生一个停止位
    bsp_DelayMS(5);
}
/*
*********************************************************************************************************
*   函 数 名: AD5245_SD
*   功能说明: 关断模式
*   形    参: addr: 参考写从机地址字节点定义
*   返 回 值: 无
*********************************************************************************************************
*/

void AD5245_SD(uint8_t addr)
{
    IIC_Start();
    IIC_Send_Byte(addr);                 //发送从机地址
    IIC_Wait_Ack();
    IIC_Send_Byte(SD_SET);               //关断指令
    IIC_Wait_Ack();
    IIC_Send_Byte(0x00);                 //发送设置值
    IIC_Wait_Ack();
    IIC_Stop();                          //产生一个停止位
    bsp_DelayMS(5);
}
