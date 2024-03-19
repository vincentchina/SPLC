//将三菱plc程序转换成dsp能直接运行的程序
#include "base.h"
#include "plcc.h"
//#include "plc_instruction.h"
#include "Memchecker.h"

int GetNextData( byte** pPlcStep, byte** pGet )
{
	byte n;
	*pGet = *pPlcStep;
	n = **pGet;
	*pPlcStep += n - 1;
	if( n != **pPlcStep )
		return -1;
	++( *pPlcStep );
	return 0;
}
void SkipData( byte** pPlcStep )
{
	*pPlcStep += **pPlcStep;
}

static int ErrorToCoil( LPPLCCData pData,byte* pos )
{
	printf( "Wpg2Str\n不支持的元件类型，或者是未知类型!\n位置：0x%x\n", pos - pData->pSource );
	return -1;
}

static int ToCoil( byte** ppstr, FILE* fout )
{
#define OSZ	5
	int16* pu16;
	int32* pu32;
	int32 addr;
	byte* pstr = *ppstr;
	byte n = *pstr - 0x02 - 1;
	char strCoil[32];
	char stro[OSZ];
	char* pstro;

	LPCoilItem pCoil; 
	if( n < 1 )
		return -1;
	SkipData( ppstr );
	++pstr;
	pCoil = GetCoilItemByWpg( *pstr );
	if( pCoil == NULL )
		return -1;
	strcpy( strCoil, pCoil->str );
	++pstr;
	if( n == 2 )
	{
		pu16 = (int16*)pstr;
		addr = *pu16;
	}else if( n == 1)
		addr = *pstr;
	else if( n == 3 )
	{
		pu32 = (int32*)pstr;
		addr = ( *pu32 ) & 0x00ffffff;
	}else if( n == 4 )
	{
		pu32 = (int32*)pstr;
		addr = *pu32;
	}else
		return -1;
	//if( scoil_V != pCoil->uwpg )
	//	fwrite( " ",1, 1,fout );
	if( scoilX == pCoil->uwpg || scoilY == pCoil->uwpg )
	{
		pstro = stro + OSZ - 1;
		*pstro = 0;
		while( addr )
		{
			--pstro;
			*pstro = ( addr % 8 ) + '0';
			addr /= 8;
		}
		sprintf( strCoil + 1,"%03s",pstro );
	}else if( scoil_V == pCoil->uwpg )
	{
		ToCoil( ppstr,fout );
		sprintf( strCoil + 1,"%d",addr );
	}else
		sprintf( strCoil + 1,"%d",addr );
	fwrite( strCoil,1, strlen(strCoil),fout );
	return 0;
}

static int ErrorIns( LPPLCCData pData, byte* pos )
{
	printf( "Wpg2Str\n不支持的指令，或者是未知指令!\n位置：0x%x\n", pos - pData->pSource );//不支持的指令，未知指令
	return -1;
}

int Wpg2Str( LPPLCCData pData )
{
	int n;
	int nArg;
	uint32 uwpg;
	LPInsItem pInsItem;
	byte* ins;
	char* pstrIns;
	byte* pstrEnd = pData->pSource + pData->szSource;
	byte* pstr = pData->pSource;

	if( !initInsHash() )
		return -1;
	pstr += pData->offsetWPG;
	while( pstr < pstrEnd )
	{
		n = *pstr - 0x02;
		ins = pstr + 1;
		SkipData( &pstr );
		uwpg = 0;
		nArg = n;
		while( nArg-- )
		{
			uwpg <<= 8;
			uwpg |= *ins++;
		}
		if( n )
		{
			pInsItem = GetInsItemByWpg( uwpg );
			if( pInsItem == NULL )
				return ErrorIns( pData, ins );
			//特殊指令
			if( pInsItem->uwpg != sinsP )
				fwrite( pInsItem->str,1,strlen(pInsItem->str),pData->fout );
			nArg = pInsItem->argc;
			while( nArg-- )
			{
				if( pInsItem->uwpg != sinsP )
					fwrite( "\t\t",2, 1,pData->fout );
				if( ToCoil( &pstr, pData->fout ) == -1 )
					return ErrorToCoil( pData, ins );
			}
			if( pInsItem->uwpg == sinsEND )
				break;
		}else
			fwrite( "NOP",1,3,pData->fout );
		fwrite( "\n",1,1,pData->fout );
	}
	return 0;
}
