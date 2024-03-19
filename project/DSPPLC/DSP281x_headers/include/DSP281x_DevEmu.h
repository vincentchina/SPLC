//###########################################################################
//
// FILE:   DSP281x_DevEmu.h
//
// TITLE:  DSP281x Device Emulation Register Definitions.
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  1.00| 11 Sep 2003 | L.H. | Changes since previous version (v.58 Alpha)
//      |             |      | As of Rev C silicon, DEVICEID is a 16-bit
//      |             |      |    register
//      |             |      | Commented out the DFT test registers.  These
//      |             |      |    are not needed as of Rev C silicon.
//###########################################################################
// $TI Release: $
// $Release Date: $
//###########################################################################

#ifndef DSP281x_DEV_EMU_H
#define DSP281x_DEV_EMU_H

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Device Emulation Register Bit Definitions:
//
// Device Configuration Register Bit Definitions
struct DEVICECNF_BITS  {     // bits  description
   Uint16 rsvd1:3;           // 2:0   reserved
   Uint16 VMAPS:1;           // 3     VMAP Status
   Uint16 rsvd2:1;           // 4     reserved
   Uint16 XRSn:1;            // 5     XRSn Signal Status
   Uint16 rsvd3:10;          // 15:6
   Uint16 rsvd4:3;           // 18:6
   Uint16 ENPROT:1;          // 19    Enable/Disable pipeline protection
   Uint16 rsvd5:12;          // 31:20 reserved
};

union DEVICECNF_REG {
   Uint32                 all;
   struct DEVICECNF_BITS  bit;
};


// Device ID Register Bit Definitions
struct DEVICEID_BITS  {      // bits  description
   Uint16 REVID:16;          // 15:0  Silicon revision
};

union DEVICEID_REG {
   Uint16                all;
   struct DEVICEID_BITS  bit;
};

// PARTID Register Bit Definitions
struct PARTID_BITS   {  // bits  description
   Uint16 PARTNO:8;     // 7:0   Part Number
   Uint16 PARTTYPE:8;   // 15:8  Part Type
};

union PARTID_REG {
   Uint16              all;
   struct PARTID_BITS  bit;
};

struct DEV_EMU_REGS {
   union DEVICECNF_REG DEVICECNF;  // device configuration
   union PARTID_REG    PARTID;     // Part ID
   union DEVICEID_REG  DEVICEID;   // Device ID
   Uint16              PROTSTART;  // Write-Read protection start
   Uint16              PROTRANGE;  // Write-Read protection range
   Uint16              rsvd2[202];
// These registers are no longer needed for Rev C and beyond
// Uint16              M0RAMDFT;
// Uint16              M1RAMDFT;
// Uint16              L0RAMDFT;
// Uint16              L1RAMDFT;
// Uint16              H0RAMDFT;
};

#define REVID          DEVICEID   // REVID replaces DEVICEID starting 281x header files v1.10
                                  // use #define to maintain code compatibility with v1.00 and older



//---------------------------------------------------------------------------
// Device Emulation Register References & Function Declarations:
//
extern volatile struct DEV_EMU_REGS DevEmuRegs;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  // end of DSP281x_DEV_EMU_H definition

//===========================================================================
// No more.
//===========================================================================
