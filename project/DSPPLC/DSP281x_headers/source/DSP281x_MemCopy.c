// TI File $Revision: /main/2 $
// Checkin $Date: April 29, 2005   11:09:17 $
//###########################################################################
//
// FILE:	DSP281x_MemCopy.c
//
// TITLE:	Memory Copy Utility
//
// ASSUMPTIONS:
//
//          
//
// DESCRIPTION:
//
//          This function will copy the specified memory contents from
//          one location to another. 
// 
//          Uint16 *SourceAddr        Pointer to the first word to be moved
//                                    SourceAddr < SourceEndAddr
//          Uint16* SourceEndAddr     Pointer to the last word to be moved
//          Uint16* DestAddr          Pointer to the first destination word
//
//          No checks are made for invalid memory locations or that the
//          end address is > then the first start address.
// 
//          

//###########################################################################
// $TI Release:$
// $Release Date:$
//###########################################################################

#include "DSP281x_Device.h"

void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr)
{
    while(SourceAddr < SourceEndAddr)
    { 
       *DestAddr++ = *SourceAddr++;
    }
    return;
}
