//###########################################################################
//
// FILE:	DSP281x_McBSP.c
//
// TITLE:	DSP281x Device McBSP Initialization & Support Functions.
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  2.00| 30 Mar 2007 | H.J. | Added new init code, Delay routine
//
// $TI Release: Internal Release 2 INT2 $
// $Release Date: January 9, 2007 $
//###########################################################################

#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File


#define MCBSP_SRG_FREQ   150E6/4  	// SRG input is default LSPCLK for this example. (See note 1 at end)
#define MCBSP_SRG_PERIOD 1/MCBSP_SRG_FREQ
#define MCBSP_INIT_DELAY (2 * MCBSP_SRG_PERIOD)/CPU_RATE  // Delay in McBSP initialization must be at least 2 SRG clock cycles

void delay_loop(void);					// Delay function used while initializing the registers

//---------------------------------------------------------------------------
// InitMcbsp: This function initializes the McBSP to a known state.
//---------------------------------------------------------------------------

void InitMcbspGpio(void)
{
	EALLOW;
	// Enable the GPIO pins for McBSP operation.
	GpioMuxRegs.GPFMUX.bit.MCLKRA_GPIOF9 = 1;
	GpioMuxRegs.GPFMUX.bit.MCLKXA_GPIOF8 = 1;
	GpioMuxRegs.GPFMUX.bit.MDRA_GPIOF13 = 1;
	GpioMuxRegs.GPFMUX.bit.MDXA_GPIOF12 = 1;
	GpioMuxRegs.GPFMUX.bit.MFSRA_GPIOF11 = 1;
	GpioMuxRegs.GPFMUX.bit.MFSXA_GPIOF10 = 1;
	EDIS;
}




void InitMcbsp(void)
{
 	EALLOW;


// McBSP-A register settings

    McbspaRegs.SPCR2.all=0x0000;		// Reset FS generator, sample rate generator & transmitter
	McbspaRegs.SPCR1.all=0x0000;		// Reset Receiver, Right justify word
    McbspaRegs.SPCR1.bit.DLB = 1;       // Enable DLB mode. Comment out for non-DLB mode.

	McbspaRegs.MFFINT.all=0x0;			// Disable all interrupts
    McbspaRegs.MFFST.all=0x0;  			// Clear all status bits

    McbspaRegs.RCR2.all=0x0;			// Single-phase frame, 1 word/frame, No companding	(Receive)
    McbspaRegs.RCR1.all=0x0;

    McbspaRegs.XCR2.all=0x0;			// Single-phase frame, 1 word/frame, No companding	(Transmit)
    McbspaRegs.XCR1.all=0x0;

    McbspaRegs.SRGR2.bit.CLKSM = 1;		// CLKSM=1 (If SCLKME=0, i/p clock to SRG is LSPCLK)
	McbspaRegs.SRGR2.bit.FPER = 31;		// FPER = 32 CLKG periods

    McbspaRegs.SRGR1.bit.FWID = 0;      // Frame Width = 1 CLKG period
    McbspaRegs.SRGR1.bit.CLKGDV = 0;	// CLKG frequency = LSPCLK/(CLKGDV+1)

    McbspaRegs.PCR.bit.FSXM = 1;		// FSX generated internally, FSR derived from an external source
	McbspaRegs.PCR.bit.CLKXM = 1;		// CLKX generated internally, CLKR derived from an external source

	McbspaRegs.SPCR2.bit.XRST=1;       	// Enable Transmitter
    McbspaRegs.SPCR1.bit.RRST=1;		// Enable Receiver
    delay_loop();
    McbspaRegs.SPCR2.all |=0x00C0;     	// Frame sync & sample rate generators pulled out of reset

}

void InitMcbspa8bit(void)
{
    McbspaRegs.RCR1.bit.RWDLEN1=0;     // 8-bit word
    McbspaRegs.XCR1.bit.XWDLEN1=0;     // 8-bit word
}

void InitMcbspa12bit(void)
{
    McbspaRegs.RCR1.bit.RWDLEN1=1;     // 12-bit word
    McbspaRegs.XCR1.bit.XWDLEN1=1;     // 12-bit word
}

void InitMcbspa16bit(void)
{
    McbspaRegs.RCR1.bit.RWDLEN1=2;      // 16-bit word
    McbspaRegs.XCR1.bit.XWDLEN1=2;      // 16-bit word
}

void InitMcbspa20bit(void)
{
    McbspaRegs.RCR1.bit.RWDLEN1=3;     // 20-bit word
    McbspaRegs.XCR1.bit.XWDLEN1=3;     // 20-bit word
}

void InitMcbspa24bit(void)
{
    McbspaRegs.RCR1.bit.RWDLEN1=4;     // 24-bit word
    McbspaRegs.XCR1.bit.XWDLEN1=4;     // 24-bit word
}

void InitMcbspa32bit(void)
{
    McbspaRegs.RCR1.bit.RWDLEN1=5;     // 32-bit word
    McbspaRegs.XCR1.bit.XWDLEN1=5;     // 32-bit word
}

void delay_loop(void)					// Delay function used while initializing the registers
{
    long      i;
    for (i = 0; i < MCBSP_INIT_DELAY; i++) {}
}

//===========================================================================
// No more.
//===========================================================================

/* Note 1

The parameter MCBSP_SRG_FREQ denotes the input frequency to the SRG. It is
set to the default LSPCLK frequency (150 MHz/ 4 = 37.5 MHz) in this file.
Change this value to reflect the correct frequency in your application.

*/









