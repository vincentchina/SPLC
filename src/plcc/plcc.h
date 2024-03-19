#ifndef __PLC2C2_H__
#define __PLC2C2_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tagPLCCData{
	unsigned char*	pSource;
	int				szSource;
	FILE*			fout;
	unsigned int	offsetWPG;
	int				bWPG:		1;
	int				bToCCode:	1;
	int				bToHex:		1;
	int				bDebug:1;
}PLCCData,*LPPLCCData;

int Str2c( LPPLCCData pData );

int Wpg2Str( LPPLCCData pData );
int Bin2Str( LPPLCCData pData );

void Str2BinSetHex( int bHex );
int Str2Bin( LPPLCCData pData );

#ifdef __cplusplus
}
#endif
#endif