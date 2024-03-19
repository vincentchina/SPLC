#ifndef __PLCINSSYS_H__
#define __PLCINSSYS_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include "dllpreHead.h"
#include "instruction.h"

typedef unsigned short PLCUINT16;

#define MAX_INS_ARGC	4

#define MAXSZ_ARG	10

#define ARG_COIL_X	0x001
#define ARG_COIL_Y	0x002
#define ARG_COIL_M	0x004
#define ARG_COIL_S	0x008
#define ARG_COIL_T	0x010
#define ARG_COIL_C	0x020
#define ARG_COIL_D	0x040
#define ARG_COIL_P	0x080
#define ARG_COIL_K	0x100
#define ARG_COIL_V	0x200
#define ARG_COIL_N	0x400
#define ARG_COIL_G	0x800

#define ARG_COIL_BIT	( ARG_COIL_X | ARG_COIL_Y | ARG_COIL_M | ARG_COIL_S | ARG_COIL_T | ARG_COIL_C )
#define ARG_COIL_DATA	( ARG_COIL_D | ARG_COIL_K | ARG_COIL_V )

typedef union tagPLCIns{
	PLCUINT16 all;
	struct{
		PLCUINT16 ins:4;
		PLCUINT16 itype:4;
		PLCUINT16 iclass:4;
		PLCUINT16 iargc:4;
	};
}PLCIns,*LPPLCIns;

typedef union tagPLCCoil{
	PLCUINT16 all;
	struct{
		PLCUINT16 coil:4;
		PLCUINT16 ctype:4;
		PLCUINT16 cargc:4;
		PLCUINT16 reserve:4;
	};
}PLCCoil,*LPPLCCoil;

typedef struct tagInsItem{
	PLCIns bin;
	unsigned long uwpg;
	const char* str;
	int argc;
	int bDK;
	unsigned short argv[ MAX_INS_ARGC ];
	unsigned long hash;
}InsItem,*LPInsItem;

typedef struct tagCoilItem{
	PLCCoil bin;
	unsigned short uwpg;
	const char* str;
}CoilItem,*LPCoilItem;

int LIBX_API initInsHash();
LPInsItem LIBX_API GetInsItemByStr( const char* str );
LPInsItem LIBX_API GetNextInsItemByStr( LPInsItem pItem );
LPInsItem LIBX_API GetInsItemByBin( PLCIns bin );
LPInsItem LIBX_API GetInsItemByWpg( unsigned long uwpg );

LPCoilItem LIBX_API GetCoilItemByStr( const char* str );
LPCoilItem LIBX_API GetNextCoilItemByStr( LPCoilItem pItem );
LPCoilItem LIBX_API GetCoilItemByBin( PLCCoil bin );
LPCoilItem LIBX_API GetCoilItemByWpg( unsigned short uwpg );

long OpenPLCFileW( FILE** pFile, const wchar_t* plcFile, wchar_t** err );
long OpenPLCFile( FILE** pFile, const char* plcFile, char** err );
int GetInisLine(char** pstr, char* pList[], int szlist, int* plineGet );

#ifdef __cplusplus
}
#endif
#endif	
