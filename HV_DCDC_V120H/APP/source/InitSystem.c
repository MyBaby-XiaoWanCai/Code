/*
 * InitSystem.c
 *
 *  Created on: 2021年7月19日
 *      Author: JC
 */

#include "F28x_Project.h"

Uint16 InterruptCount1 = 0,InterruptCount2 = 0,InterruptCount3 = 0,InterruptCount4 = 0;

interrupt void cpu_timer0_isr(void);//1s

interrupt void xint1_isr(void);
interrupt void xint2_isr(void);
interrupt void xint3_isr(void);
interrupt void xint4_isr(void);

void InitSystem(void)
{
    Uint16 i;
    DINT;
    // Step 1. Initialize System Control registers
    InitSysCtrl();

    //********************************************
    // Step 2. Select GPIO for the specific application
    InitGpio();
    InitGPIO_BSP();
    InitEPwmGpio();   // EPWM1A  EPWM1B  through EPWM9
   //********************************************

    //Step 3. Initialize PIE to known state
    InitPieCtrl();
    //********************************************
    //Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;


    //Step 4. Initialize the PIE vector table with pointers to the shell Interrupt
    InitPieVectTable();
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
   // PieCtrlRegs.PIEIER9.bit.INTx5 = 1;
    PieCtrlRegs.PIEIER9.bit.INTx7 = 1;
  //  PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
  //  PieCtrlRegs.PIEIER1.bit.INTx3 = 1;
 //   PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          // Enable PIE Group 1 INT4
    PieCtrlRegs.PIEIER1.bit.INTx5 = 1;          // Enable PIE Group 1 INT5
    PieCtrlRegs.PIEIER12.bit.INTx1 = 1;          // Enable PIE Group 12 INT1
    PieCtrlRegs.PIEIER12.bit.INTx2 = 1;
    // Enable PIE Group 12 INT2
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.CANB0_INT = canbISR;
   // PieVectTable.ADCA1_INT = &adca1_isr;
    PieVectTable.SCIA_RX_INT  = &SCIAReceiveInterrupt;
 //   PieVectTable.ADCC1_INT = &adcc1_isr;
    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
 //   PieVectTable.TIMER2_INT      = &Cputimer2Interrupt;
    PieVectTable.XINT1_INT = &xint1_isr;
    PieVectTable.EPWM1_INT  = &EPWM1_INT_ISR_BSP;
  //  PieVectTable.XINT2_INT = &xint2_isr;
  //  PieVectTable.XINT3_INT = &xint3_isr;
  //  PieVectTable.XINT4_INT = &xint4_isr;
    EDIS;    // This is needed to disable write to EALLOW protected registers

  //  IER |= M_INT14; //for Timer2
    IER |= M_INT1; //使能第一组中断
    IER |= M_INT9;
    IER |= M_INT12;
    IER |= M_INT3;//使能PWM中断
    //********************************************

     //Step 5. Initalize EMIF
     InitEmif1();   //外部并口读AD初始化
     //********************************************
     //Step 6. Initlize EV
      HRPWM1_Config();
      HRPWM2_Config();
      HRPWM3_Config();
      HRPWM4_Config();
      HRPWM5_Config();
      HRPWM6_Config();
      HRPWM7_Config();
      HRPWM8_Config();
      PWM9_Config();
      PWM10_Config();
      PWM11_Config();
      PWM12_Config();
      setupHrpwmMepScaleFactor();
      //********************************************

      //Step 7. Initlize Can
      InitCAN_BSP(); //CAN底层外设初始化
      //********************************************

      //Step 8. Initlize ADC
      InitADC_BSP(); //AD外设初始化
      InitAD7124();
      //********************************************

      //Step 9. Initlize I2C
      InitI2C_BSP(); //I2C外设初始化
      //********************************************

      //Step 10. Initlize I2C
       InitSCI_BSP(); //I2C外设初始化
     //********************************************

       //Step 10. Initlize I2C
       InitFlash_BSP(); //I2C外设初始化
     //********************************************
       ADCA_Sample.resultsIndex1=0;
       ADCA_Sample.resultsIndex2=0;
       ADCA_Sample.resultsIndex3=0;
       ADCA_Sample.resultsIndex4=0;
       ADCA_Sample.ADCA_CH_SUM0=0;
       ADCA_Sample.ADCA_CH_SUM1=0;
       ADCA_Sample.ADCA_CH_SUM2=0;
       ADCA_Sample.ADCA_CH_SUM3=0;
       ADCA_Sample.ADCA_CH_SUM4=0;
       ADCA_Sample.ADCA_CH_SUM5=0;
       ADCA_Sample.ADCA_CH_SUM6=0;
       ADCA_Sample.ADCA_CH_SUM7=0;
       for(i=0;i<5;i++)
       {
             ADCA_Sample.ADCA_CH0[i]=0;
             ADCA_Sample.ADCA_CH1[i]=0;
             ADCA_Sample.ADCA_CH2[i]=0;
             ADCA_Sample.ADCA_CH3[i]=0;
             ADCA_Sample.ADCA_CH4[i]=0;
             ADCA_Sample.ADCA_CH5[i]=0;
             ADCA_Sample.ADCA_CH6[i]=0;
             ADCA_Sample.ADCA_CH7[i]=0;
       }

      CLA_configClaMemory();
      CLA_initCpu1Cla1();

      InitCpuTimers();
      ConfigCpuTimer(&CpuTimer2, 200, 100);//100us中断1次
      ConfigCpuTimer(&CpuTimer0, 200, 1000);


}

//cpu_timer0_isr
interrupt void cpu_timer0_isr(void)//1s
{
   CpuTimer0.InterruptCount++;
   //IdRef+=0.1;
  // Cla1ForceTask1andWait();
   // Acknowledge this interrupt to receive more interrupts from group 1
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}

interrupt void xint1_isr(void)
{

    if(AD7124_Start_Read==1)
    {
      //  ConfigSPIInit();
        AD7124_Process();
      //  ExintInit();
    }
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
   // DELAY_US(1000);
   // if(GpioDataRegs.GPADAT.bit.GPIO12 == 0) InterruptCount1++;
   // if(InterruptCount1 > 9) InterruptCount1 = 0;

}

interrupt void xint2_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    DELAY_US(1000);
    if(GpioDataRegs.GPADAT.bit.GPIO13 == 0) InterruptCount2++;
    if(InterruptCount2 > 9) InterruptCount2 = 0;

}

interrupt void xint3_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
    DELAY_US(1000);
    if(GpioDataRegs.GPADAT.bit.GPIO14 == 0) InterruptCount3++;
    if(InterruptCount3 > 9) InterruptCount3 = 0;

}

interrupt void xint4_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
    DELAY_US(1000);
    if(GpioDataRegs.GPADAT.bit.GPIO15 == 0) InterruptCount4++;
    if(InterruptCount4 > 9) InterruptCount4 = 0;

}

// cla1Isr1 - CLA1 ISR 1
__interrupt void cla1Isr1 ()
{
    // Acknowledge the end-of-task interrupt for task 1
    PieCtrlRegs.PIEACK.all = M_INT11;
}
__interrupt void cla1Isr2 ()
{
    asm(" ESTOP0");
}
__interrupt void cla1Isr3 ()
{
    asm(" ESTOP0");
}
__interrupt void cla1Isr4 ()
{
    asm(" ESTOP0");
}
__interrupt void cla1Isr5 ()
{
    asm(" ESTOP0");
}
__interrupt void cla1Isr6 ()
{
    asm(" ESTOP0");
}
__interrupt void cla1Isr7 ()
{
    asm(" ESTOP0");
}
__interrupt void cla1Isr8 ()
{
    // Acknowledge the end-of-task interrupt for task 8
    PieCtrlRegs.PIEACK.all = M_INT11;
}

// CLA_configClaMemory - Configure the CLA memory
void CLA_configClaMemory(void)
{
    extern uint32_t Cla1funcsRunStart, Cla1funcsLoadStart, Cla1funcsLoadSize;
    EALLOW;
   // #ifdef _FLASH
    // Copy over code from FLASH to RAM
    memcpy((uint32_t *)&Cla1funcsRunStart, (uint32_t *)&Cla1funcsLoadStart,
           (uint32_t)&Cla1funcsLoadSize);
 //   #endif //_FLASH
    // Initialize and wait for CLA1ToCPUMsgRAM
    MemCfgRegs.MSGxINIT.bit.INIT_CLA1TOCPU = 1;
    while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CLA1TOCPU != 1){};
    // Initialize and wait for CPUToCLA1MsgRAM
    MemCfgRegs.MSGxINIT.bit.INIT_CPUTOCLA1 = 1;
    while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CPUTOCLA1 != 1){};
    // Select LS5RAM to be the programming space for the CLA
    // First configure the CLA to be the master for LS5 and then
    // set the space to be a program block
    MemCfgRegs.LSxMSEL.bit.MSEL_LS5 = 1;
    MemCfgRegs.LSxMSEL.bit.MSEL_LS4 = 1;
    MemCfgRegs.LSxMSEL.bit.MSEL_LS3 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS5 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS4 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS3 = 1;
    //Next configure LS0RAM and LS1RAM as data spaces for the CLA
    // First configure the CLA to be the master for LS0(1) and then
    // set the spaces to be code blocks
    MemCfgRegs.LSxMSEL.bit.MSEL_LS0 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS0 = 0;
    MemCfgRegs.LSxMSEL.bit.MSEL_LS1 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS1 = 0;
    EDIS;
}

// CLA_initCpu1Cla1 - Initialize CLA1 task vectors and end of task interrupts
void CLA_initCpu1Cla1(void)
{
    // Compute all CLA task vectors
    // On Type-1 CLAs the MVECT registers accept full 16-bit task addresses as
    // opposed to offsets used on older Type-0 CLAs
    EALLOW;
    Cla1Regs.MVECT1 = (uint16_t)(&Cla1Task1);
    Cla1Regs.MVECT2 = (uint16_t)(&Cla1Task2);
    Cla1Regs.MVECT3 = (uint16_t)(&Cla1Task3);
    Cla1Regs.MVECT4 = (uint16_t)(&Cla1Task4);
    Cla1Regs.MVECT5 = (uint16_t)(&Cla1Task5);
    Cla1Regs.MVECT6 = (uint16_t)(&Cla1Task6);
    Cla1Regs.MVECT7 = (uint16_t)(&Cla1Task7);
    Cla1Regs.MVECT8 = (uint16_t)(&Cla1Task8);
    // Enable the IACK instruction to start a task on CLA in software
    // for all  8 CLA tasks. Also, globally enable all 8 tasks (or a
    // subset of tasks) by writing to their respective bits in the
    // MIER register
    Cla1Regs.MCTL.bit.IACKE = 1;
    Cla1Regs.MIER.all = 0x00FF;
   // Cla1ForceTask8();
    // Configure the vectors for the end-of-task interrupt for all
    // 8 tasks
    PieVectTable.CLA1_1_INT  = &cla1Isr1;
    PieVectTable.CLA1_2_INT  = &cla1Isr2;
    PieVectTable.CLA1_3_INT  = &cla1Isr3;
    PieVectTable.CLA1_4_INT  = &cla1Isr4;
    PieVectTable.CLA1_5_INT  = &cla1Isr5;
    PieVectTable.CLA1_6_INT  = &cla1Isr6;
    PieVectTable.CLA1_7_INT  = &cla1Isr7;
    PieVectTable.CLA1_8_INT  = &cla1Isr8;
    // Set the Software Trigger as the trigger for task 1
    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK1 = 1;
    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK2 = 2;
    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK3 = 3;
    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK4 = 4;
    // Enable CLA interrupts at the group and subgroup levels
  //  PieCtrlRegs.PIEIER11.all = 0xFFFF;
  //  IER |= (M_INT11 );
    EDIS;    // This is needed to disable write to EALLOW protected registers
}
