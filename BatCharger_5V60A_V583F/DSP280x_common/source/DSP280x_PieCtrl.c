// TI File $Revision: /main/1 $
// Checkin $Date: December 1, 2004   11:12:03 $
//###########################################################################
//
// FILE:	DSP280x_PieCtrl.c
//
// TITLE:	DSP280x Device PIE Control Register Initialization Functions.
//
//###########################################################################
// $TI Release: DSP280x V1.30 $
// $Release Date: February 10, 2006 $
//###########################################################################

#include "..\..\DSP280x_headers\include\DSP280x_Device.h"     // DSP280x Headerfile Include File
#include "..\include\DSP280x_Examples.h"   // DSP280x Examples Include File

//---------------------------------------------------------------------------
// InitPieCtrl: 
//---------------------------------------------------------------------------
// This function initializes the PIE control registers to a known state.
//
void InitPieCtrl(void)
{
    // Disable Interrupts at the CPU level:
    DINT;

    // Disable the PIE
    PieCtrlRegs.PIECTRL.bit.ENPIE = 0;

	// Clear all PIEIER registers:
	PieCtrlRegs.PIEIER1.all = 0;
	PieCtrlRegs.PIEIER2.all = 0;
	PieCtrlRegs.PIEIER3.all = 0;	
	PieCtrlRegs.PIEIER4.all = 0;
	PieCtrlRegs.PIEIER5.all = 0;
	PieCtrlRegs.PIEIER6.all = 0;
	PieCtrlRegs.PIEIER7.all = 0;
	PieCtrlRegs.PIEIER8.all = 0;
	PieCtrlRegs.PIEIER9.all = 0;
	PieCtrlRegs.PIEIER10.all = 0;
	PieCtrlRegs.PIEIER11.all = 0;
	PieCtrlRegs.PIEIER12.all = 0;

	// Clear all PIEIFR registers:
	PieCtrlRegs.PIEIFR1.all = 0;
	PieCtrlRegs.PIEIFR2.all = 0;
	PieCtrlRegs.PIEIFR3.all = 0;	
	PieCtrlRegs.PIEIFR4.all = 0;
	PieCtrlRegs.PIEIFR5.all = 0;
	PieCtrlRegs.PIEIFR6.all = 0;
	PieCtrlRegs.PIEIFR7.all = 0;
	PieCtrlRegs.PIEIFR8.all = 0;
	PieCtrlRegs.PIEIFR9.all = 0;
	PieCtrlRegs.PIEIFR10.all = 0;
	PieCtrlRegs.PIEIFR11.all = 0;
	PieCtrlRegs.PIEIFR12.all = 0;


}	

//---------------------------------------------------------------------------
// EnableInterrupts: 
//---------------------------------------------------------------------------
// This function enables the PIE module and CPU interrupts
//
void EnableInterrupts()
{

    // Enable the PIE
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    		
	// Enables PIE to drive a pulse into the CPU 
	PieCtrlRegs.PIEACK.all = 0xFFFF;  

	// Enable Interrupts at the CPU level 

//---------------------------------------
	//EPwm1Regs.ETSEL.bit.INTEN = 1;     // Enable EPWM1INT generation 	
	//EPwm1Regs.ETCLR.bit.INT = 1;       // Enable more interrupts
	
	//EALLOW;
	//SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	//EDIS; 
    //WD_Enable();
//------------------------------------------------------------
	//EQep1Regs.QEPCTL.bit.QPEN=1; 		//1:QEP enable
	//EQep1Regs.QEPCTL.bit.UTE=1; 		// 1:Unit Timeout Enable

	//EQep2Regs.QEPCTL.bit.QPEN=1; 		//1:QEP enable
	//EQep2Regs.QEPCTL.bit.UTE=1; 		// 1:Unit Timeout Enable 
	//EQep2Regs.QCAPCTL.bit.CEN=1; 		// QEP Capture Enable
	//EQep2Regs.QEINT.bit.UTO=1;			//Unit time out interrupt enable
//--------------------------------------------
	//XIntruptRegs.XINT1CR.all=3;			//上升沿,enable int
	// Enable SCIARX_INT:		INT9
	PieCtrlRegs.PIEIER9.bit.INTx1 = 1;

	// Enable EQEP2_INT:		INT5
	//PieCtrlRegs.PIEIER5.bit.INTx2 = 1;

	// Enable PWM1 int:			INT3
	//PieCtrlRegs.PIEIER3.bit.INTx1 = 1;

	// Enable PWM1 TZINT:		INT2		
	//PieCtrlRegs.PIEIER2.bit.INTx1 = 1;

	// Enable XINT1:			INT1.4
	//PieCtrlRegs.PIEIER1.bit.INTx4 = 1;

	// Enable WDINT:			INT1.8
	//PieCtrlRegs.PIEIER1.bit.INTx8 = 1;
	
	// clear INT FLAG
	//EPwm1Regs.ETCLR.bit.INT = 1;
	//EQep2Regs.QCLR.bit.UTO = 1;
	//EQep2Regs.QCLR.bit.INT = 1;
	//PieCtrlRegs.PIEACK.all = 0xFFFF;
	// Enable the PIE
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
	
	IER |= M_INT9;
	
//使能中断，寄存器等//
	EINT;   // Enable Global interrupt INTM,使能所有中断
	ERTM;   // Enable Global realtime interrupt DBGM

}


//===========================================================================
// End of file.
//===========================================================================
