//############################################################################
//
// FILE:   Example_281x_Flash_to_RAM_nonBIOS.c
//
// TITLE:  DSP281x Flash to RAM Example 
//
// ASSUMPTIONS:
//
//    This program uses the DSP281x header files already included in the  
// 	  download. 
//
//    As supplied, this project is configured for "boot to FLASH" 
//    operation.  The 281x Boot Mode table is shown below.  
//    For information on configuring the boot mode of an eZdsp, 
//    please refer to the documentation included with the eZdsp,  
//
//       Boot      GPIOF4     GPIOF12    GPIOF3    GPIOF2 
//       Mode      SCITXDA    MDXA       SPISTEA   SPICLK
//       -------------------------------------------------
//       Flash       1          X          X         X    <- "boot to FLASH"
//       SPI         0          1          X         X
//       SCI         0          0          1         1
//       H0          0          0          1         0
//       OTP         0          0          0         1
//       PARALLEL    0          0          0         0
//
//    The program must first be compiled and then programmed into the
//    flash. 
//
// DESCRIPTION:
//	 
//	  This project provides an example of how copy code from flash to RAM directly
//	  after code startup. This will allow all code to be ran from RAM directly after 
//    booting in standalone mode. GPIO34 is toggled at a specific time selected within 
//	  this file in the ePWM1 ISR.  
//		
//############################################################################		
// Original Author: Tim Love
// Release Date: March 2008
//############################################################################


#include "DSP281x_Device.h"     // Headerfile Include File
#include "DSP281x_Examples.h"   // Examples Include File

// Specify LED Toggle Rate 
#define EVA_TIMER_PRD 63025  	// EVA Timer Period (LED Toggle .75 sec)
//#define EVA_TIMER_PRD 42016  	// EVA Timer Period (LED Toggle .5 sec)
//#define EVA_TIMER_PRD 21008   	// EVA Timer Period (LED Toggle .25 sec)

// Prototype statements for functions found within this file.
interrupt void eva_timer1_isr(void);
void init_eva_timer1(void);

void main(void)
{

// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP281x_SysCtrl.c file.
   InitSysCtrl();

// Step 2. Initalize GPIO: 
// This example function is found in the DSP281x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// InitGpio();  // Skipped for this example  

// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts 
   DINT;

// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.  
// This function is found in the DSP281x_PieCtrl.c file.
   InitPieCtrl();
   
// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt 
// Service Routines (ISR).  
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in DSP281x_DefaultIsr.c.
// This function is found in DSP281x_PieVect.c.
   InitPieVectTable();

// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.  
   EALLOW;  // This is needed to write to EALLOW protected registers
   PieVectTable.T1PINT = &eva_timer1_isr;
   EDIS;    // This is needed to disable write to EALLOW protected registers

// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP281x_InitPeripherals.c
// InitPeripherals();  // Not required for this example
   init_eva_timer1();    // For this example, initialize ePWM1 Timers

// Step 5. User specific code, enable interrupts:*/

   // Configure GPIOF14 as output to drive LED on eZdsp
   EALLOW;
   GpioMuxRegs.GPFDIR.bit.GPIOF14 = 1;
   EDIS;

   // Enable CPU INT2 which is connected to EVA TIMER 1 INT:
   IER |= M_INT2;

   // Enable EVAPINt in the PIE: Group 2 interrupt 4
   PieCtrlRegs.PIEIER2.bit.INTx4 = 1;

   // Enable global Interrupts and higher priority real-time debug events:
   EINT;   // Enable Global interrupt INTM
   ERTM;   // Enable Global realtime interrupt DBGM

// Step 6. IDLE loop. Just sit and loop forever (optional):
     
   for(;;){};

} 

void init_eva_timer1()
{

  // Initialize EVA Timer 1:
   
    // Set the Period for the GP timer 1 to EVA_TIMER_PRD;
    EvaRegs.T1PR = EVA_TIMER_PRD;       // Period
   
    // Enable Period interrupt bits for GP timer 1
    EvaRegs.EVAIMRA.bit.T1PINT = 1;
    EvaRegs.EVAIFRA.bit.T1PINT = 1;

    // Clear the counter for GP timer 1
    EvaRegs.T1CNT = 0x0000;

	// Count up, x128, internal clk, enable compare, use own period
    EvaRegs.T1CON.all = 0x1740;
}

// Interrupt routines uses in this example:
interrupt void eva_timer1_isr(void)
{
   // Toggle LED
   GpioDataRegs.GPFTOGGLE.bit.GPIOF14 = 1;

   // Note: To be safe, use a mask value to write to the entire
   // EVAIFRA register.  Writing to one bit will cause a read-modify-write
   // operation that may have the result of writing 1's to clear 
   // bits other then those intended. 
   EvaRegs.EVAIFRA.all = BIT7;

   // Acknowledge interrupt to receive more interrupts from PIE group 2
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}
