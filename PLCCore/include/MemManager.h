#ifndef __MEMMANAGER_H__
#define __MEMMANAGER_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "PLCCore.h"

static const uint16 BITMARK_SET[] = { 
	0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
	0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000,
};
static const uint16 BITMARK_CLR[] = { 
	~0x0001,~0x0002,~0x0004,~0x0008,~0x0010,~0x0020,~0x0040,~0x0080,
	~0x0100,~0x0200,~0x0400,~0x0800,~0x1000,~0x2000,~0x4000,~0x8000,
};

#define GETBIT( pMem, addr ) !!( (pMem)[ (addr) >> 4 ] & BITMARK_SET[ (addr) & 0x000f ] )

#define SETBIT( pMem, addr ) ( (pMem)[ (addr) >> 4 ] |= BITMARK_SET[ (addr) & 0x000f ] )

#define CLRBIT( pMem, addr ) ( (pMem)[ (addr) >> 4 ] &= BITMARK_CLR[ (addr) & 0x000f ] )


/*	//不再使用
api_res CreateMem( PLPLCSystem psys );*/

void ZeroMem( PLPLCSystem psys );
/*	//不再使用
uint16* CreateBitMem( uint16 sz );*/
api_res UpdateMem( PLPLCSystem psys, uint16* pByteBlock,int bytesz, uint16* pBitBlock,int bitsz );

uint16 GetBitMapSize( PLPLCSystem psys );
uint16 GetByteMapSize(  PLPLCSystem psys );

#define M_IsPLCIns( p )		( ( (LPPLCIns)p )->iargc != 0 )

enum eCoilDataType{
	eUData16,
	eUData32,
	eSData16,
	eSData32,
};
api_res M_GetCoilData( enum eCoilDataType dataType, LPRegGet regGet, LPCoilData pcData );
api_res M_LoadProgram( PLCSystem* psys );
api_res M_GetReg( mbyte** pp,LPRegGet pRegGet );

void M_DumpMem( PLPLCSystem psys );
void M_dDump( mbyte* pdata, uint16 sz );

void tiny_mest( void* mem, int value, int sz );

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __MEMMANAGER_H__ */

