/*############################################################################

 FILE:   F281x_nonBIOS_flash.cmd

 DESCRIPTION:  Linker allocation for all sections. 
############################################################################
 Author: Tim Love
 Release Date: March 2008
############################################################################*/


MEMORY
{
PAGE 0:    /* Program Memory */
           /* Memory (RAM/FLASH/OTP) blocks can be moved to PAGE1 for data allocation */
	RAM_L0L1	: origin = 0x008000, length = 0x002000	   /* on-chip RAM */
   	OTP         : origin = 0x3D7800, length = 0x000400     /* on-chip OTP */
   	FLASH_IJ    : origin = 0x3D8000, length = 0x004000     /* on-chip FLASH */
    FLASH_GH    : origin = 0x3DC000, length = 0x008000     /* on-chip FLASH */
    FLASH_EF    : origin = 0x3E4000, length = 0x008000     /* on-chip FLASH */
    FLASH_CD    : origin = 0x3EC000, length = 0x008000     /* on-chip FLASH */
    FLASH_AB    : origin = 0x3F4000, length = 0x003F80     /* on-chip FLASH */
   	CSM_RSVD    : origin = 0x3F7F80, length = 0x000076     /* Part of FLASHA.  Program with all 0x0000 when CSM is in use. */
   	BEGIN_FLASH : origin = 0x3F7FF6, length = 0x000002     /* Part of FLASHA.  Used for "boot to Flash" bootloader mode. */
   	CSM_PWL     : origin = 0x3F7FF8, length = 0x000008     /* Part of FLASHA.  CSM password locations in FLASHA */
   	RAM_H0		: origin = 0x3F8000, length = 0x002000     /* on-chip RAM */
   	ROM         : origin = 0x3FF000, length = 0x000FC0     /* Boot ROM */
   	RESET       : origin = 0x3FFFC0, length = 0x000002     /* part of boot ROM  */
   	VECTORS     : origin = 0x3FFFC2, length = 0x00003E     /* part of boot ROM  */


PAGE 1 :   /* Data Memory */
           /* Memory (RAM/FLASH/OTP) blocks can be moved to PAGE0 for program allocation */
           /* Registers remain on PAGE1                                                  */

   	RAMM0       : origin = 0x000000, length = 0x000400     /* on-chip RAM block M0 */
   	BOOT_RSVD   : origin = 0x000400, length = 0x000080     /* Part of M1, BOOT rom will use this for stack */
   	RAMM1       : origin = 0x000480, length = 0x000380     /* on-chip RAM block M1 */
}

/**************************************************************/
/* Link all user defined sections                             */
/**************************************************************/
SECTIONS
{

/*** Code Security Password Locations ***/
   	csmpasswds      : > CSM_PWL     	PAGE = 0		/* Used by file CSMPasswords.asm */
   	csm_rsvd        : > CSM_RSVD    	PAGE = 0		/* Used by file CSMPasswords.asm */		

/*** User Defined Sections ***/
   	codestart       : > BEGIN_FLASH,	PAGE = 0        /* Used by file CodeStartBranch.asm */
   	wddisable		: > FLASH_AB,		PAGE = 0		/* Used by file CodeStartBranch.asm */		
  	copysections	: > FLASH_AB,		PAGE = 0		/* Used by file SectionCopy.asm */       

/* .reset is a standard section used by the compiler.  It contains the */ 
/* the address of the start of _c_int00 for C Code.   /*
/* When using the boot ROM this section and the CPU vector */
/* table is not needed.  Thus the default type is set here to  */
/* DSECT  */ 
	.reset         	: > RESET,      	PAGE = 0, TYPE = DSECT
	vectors         : > VECTORS     	PAGE = 0, TYPE = DSECT

/*** Uninitialized Sections ***/
   	.stack          : > RAMM0       	PAGE = 1
   	.ebss           : > RAMM1       	PAGE = 1
   	.esysmem        : > RAMM1       	PAGE = 1

/*** Initialized Sections ***/                                          
  	.cinit			:	LOAD = FLASH_AB,	PAGE = 0	/* Load section to Flash */ 
                		RUN = RAM_H0,  		PAGE = 0    /* Run section from RAM */
                		LOAD_START(_cinit_loadstart),
                		RUN_START(_cinit_runstart),
						SIZE(_cinit_size)

	.const			:   LOAD = FLASH_AB,  	PAGE = 0    /* Load section to Flash */
                		RUN = RAM_H0, 		PAGE = 0    /* Run section from RAM */
                		LOAD_START(_const_loadstart),
                		RUN_START(_const_runstart),
						SIZE(_const_size)

	.econst			:   LOAD = FLASH_AB,  	PAGE = 0   	/* Load section to Flash */ 
                		RUN = RAM_H0,  		PAGE = 0    /* Run section from RAM */
                		LOAD_START(_econst_loadstart),
                		RUN_START(_econst_runstart),
						SIZE(_econst_size)

	.pinit			:   LOAD = FLASH_AB,  	PAGE = 0    /* Load section to Flash */
                		RUN = RAM_H0,   	PAGE = 0    /* Run section from RAM */
                		LOAD_START(_pinit_loadstart),
                		RUN_START(_pinit_runstart),
						SIZE(_pinit_size)

	.switch			:   LOAD = FLASH_AB,  	PAGE = 0   	/* Load section to Flash */ 
                		RUN = RAM_H0,   	PAGE = 0    /* Run section from RAM */
                		LOAD_START(_switch_loadstart),
                		RUN_START(_switch_runstart),
						SIZE(_switch_size)

	.text			:   LOAD = FLASH_AB, 	PAGE = 0    /* Load section to Flash */ 
                		RUN = RAM_L0L1,   	PAGE = 0    /* Run section from RAM */
                		LOAD_START(_text_loadstart),
                		RUN_START(_text_runstart),
						SIZE(_text_size)
}

/******************* end of file ************************/
