#include "stdafx.h"
#include "apis.h"
#include "PLCCore.h"

PLCCORE_API BOOL lib_API_InitSystem( LPPLCSysInitInfo pInitInfo )
{
	/*int a,b,c1,c2;
	a = -1;
	b = -1;
	M_MulNum32( a, b, (uint32*)&c1, (uint32*)&c2 );*/
	//printf("sz long long:%d\n", sizeof( long long ) );
	return ( API_InitSystem( (LPSysInitInfo)pInitInfo ) == RES_OK );
}

PLCCORE_API void lib_API_UninitSystem()
{
	return API_UninitSystem();
}

PLCCORE_API void lib_API_FeedWDG()
{
	return API_FeedWDG();
}

PLCCORE_API void lib_API_SetWDG( unsigned short ums )
{
	return API_SetWDG( ums );
}

PLCCORE_API int lib_API_LoadProgram()
{
	return (API_LoadProgram() == RES_OK);
}
PLCCORE_API int lib_API_PLCScan()
{
	return API_PLCScan();
}
PLCCORE_API int lib_API_PLCRun()
{
	return API_PLCRun();
}

PLCCORE_API unsigned char lib_API_ReadBit( unsigned char nCoil, unsigned short addr )
{
	unsigned char b = 0;
	API_ReadBit( nCoil, addr, &b, 1 );
	return b;
}
PLCCORE_API int lib_API_WriteBit( unsigned char nCoil, unsigned short addr, unsigned char d )
{
	return ( API_WriteBit( nCoil, addr, &d, 1 ) == 1 );
}

//一次只能一个字节读写
PLCCORE_API int lib_API_ReadData( unsigned char nCoil, unsigned short addr, unsigned int* pData )
{
	int sz;
	sz = 1;
	if( nCoil == api_coilDD || ( nCoil == api_coilDC && addr >= 200 ) )
		sz = 2;
	return ( API_ReadData( nCoil, addr, (unsigned short*)pData, sz ) == sz );
}

PLCCORE_API int lib_API_WriteData( unsigned char nCoil, unsigned short addr, unsigned int d )
{
	int sz;
	sz = 1;
	if( nCoil == api_coilDD || ( nCoil == api_coilDC && addr >= 200 ) )
		sz = 2;
	return ( API_WriteData( nCoil, addr, (unsigned short*)&d, sz ) == sz );
}

PLCCORE_API int lib_API_IsPLCRuning()
{
	return API_IsPLCRuning();
}
PLCCORE_API int lib_API_ProGramRead( unsigned short offset, unsigned short* pData )
{
	return ( API_ProGramRead( offset, pData, 1 ) == RES_OK );
}
PLCCORE_API int lib_API_ProGramWrite( unsigned short offset, unsigned short data )
{
	return ( API_ProGramWrite( offset, &data, 1 ) == RES_OK );
}

PLCCORE_API void lib_API_TimerUp()
{
	API_TimerUp();
}

PLCCORE_API int lib_API_Protocol( const int id, unsigned char* pData, unsigned short sz )
{
	return ( API_Protocol( id, pData, sz ) == RES_OK );
}


PLCCORE_API void lib_API_DumpMem( FILE* fp, unsigned int uFlag )
{
	PLPLCSystem pSys = GetSystem();
	if( uFlag & DUMP_MEM_X )
	{
		fwrite( pSys->R[REG_X].pState, sizeof( uint16 ),( pSys->R[REG_X].sz + 16 - 1 )  / 16, fp );
	}
	if( uFlag & DUMP_MEM_Y )
	{
		fwrite( pSys->R[REG_Y].pState, sizeof( uint16 ),( pSys->R[REG_Y].sz + 16 - 1 )  / 16, fp );
	}
	if( uFlag & DUMP_MEM_S )
	{
		fwrite( pSys->R[REG_S].pState, sizeof( uint16 ),( pSys->R[REG_S].sz + 16 - 1 )  / 16, fp );
	}
	if( uFlag & DUMP_MEM_M )
	{
		fwrite( pSys->R[REG_M].pState, sizeof( uint16 ),( pSys->R[REG_M].sz + 16 - 1 )  / 16, fp );
	}
	if( uFlag & DUMP_MEM_D )
	{
		fwrite( pSys->R[REG_D].pData, sizeof( uint16 ),pSys->R[REG_D].sz, fp );
	}
	if( uFlag & DUMP_MEM_C )
	{
		fwrite( pSys->R[REG_C].pState, sizeof( uint16 ),( pSys->R[REG_C].sz + 16 - 1 )  / 16, fp );
	}
	if( uFlag & DUMP_MEM_DC )
	{
		fwrite( pSys->R[REG_C].pData, sizeof( uint16 ),pSys->R[REG_C].sz, fp );
	}
	if( uFlag & DUMP_MEM_DT )
	{
		fwrite( pSys->R[REG_T].pData, sizeof( uint16 ),pSys->R[REG_T].sz, fp );
	}
	if( uFlag & DUMP_MEM_RAM )
	{
		fwrite( pSys->pPlcMain, sizeof( uint16 ), pSys->pPlcPrgEnd - pSys->pPlcMain, fp );
	}
	if( uFlag & DUMP_MEM_FLASH )
	{
		fwrite( pSys->pFlash, sizeof( uint16 ), pSys->szFlash, fp );
	}
}
PLCCORE_API void lib_API_LoadMem( FILE* fp, unsigned int uFlag )
{
	PLPLCSystem pSys = GetSystem();
	if( uFlag & DUMP_MEM_X )
	{
		fread( pSys->R[REG_X].pState, sizeof( uint16 ),( pSys->R[REG_X].sz + 16 - 1 )  / 16, fp );
	}
	if( uFlag & DUMP_MEM_Y )
	{
		fread( pSys->R[REG_Y].pState, sizeof( uint16 ),( pSys->R[REG_Y].sz + 16 - 1 )  / 16, fp );
	}
	if( uFlag & DUMP_MEM_S )
	{
		fread( pSys->R[REG_S].pState, sizeof( uint16 ),( pSys->R[REG_S].sz + 16 - 1 )  / 16, fp );
	}
	if( uFlag & DUMP_MEM_M )
	{
		fread( pSys->R[REG_M].pState, sizeof( uint16 ),( pSys->R[REG_M].sz + 16 - 1 )  / 16, fp );
	}
	if( uFlag & DUMP_MEM_D )
	{
		fread( pSys->R[REG_D].pData, sizeof( uint16 ),pSys->R[REG_D].sz, fp );
	}
	if( uFlag & DUMP_MEM_C )
	{
		fread( pSys->R[REG_C].pState, sizeof( uint16 ),( pSys->R[REG_C].sz + 16 - 1 )  / 16, fp );
	}
	if( uFlag & DUMP_MEM_DC )
	{
		fread( pSys->R[REG_C].pData, sizeof( uint16 ),pSys->R[REG_C].sz, fp );
	}
	if( uFlag & DUMP_MEM_DT )
	{
		fread( pSys->R[REG_T].pData, sizeof( uint16 ),pSys->R[REG_T].sz, fp );
	}
	if( uFlag & DUMP_MEM_RAM )
	{
		fread( pSys->pPlcMain, sizeof( uint16 ), pSys->pPlcPrgEnd - pSys->pPlcMain, fp );
	}
	if( uFlag & DUMP_MEM_FLASH )
	{
		fread( pSys->pFlash, sizeof( uint16 ), pSys->szFlash, fp );
	}
}
PLCCORE_API void lib_API_ResetPLC()
{
	API_ResetPLC();
}

PLCCORE_API void lib_API_StopMachine( int id )
{
	API_StopMachine( id );
}