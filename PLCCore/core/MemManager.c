#include "MemManager.h"
#include "insClass.h"

extern void _dDumpMem( PLPLCSystem psys );
extern PLCSystem g_sys;

/*
//不使用了
uint16 GetBitMapSize( PLPLCSystem psys )
{
	uint16 szBit;
	szBit = ( psys->R[REG_X].sz + 16 - 1 )  / 16;
	szBit += ( psys->R[REG_Y].sz + 16 - 1 )  / 16;
	szBit += ( psys->R[REG_M].sz + 16 - 1 )  / 16;
	szBit += ( psys->R[REG_S].sz + 16 - 1 )  / 16;
	szBit += ( psys->R[REG_T].sz + 16 - 1 )  / 16;
	szBit += ( psys->R[REG_C].sz + 16 - 1 )  / 16;
	szBit *= 2;
	szBit += ( SZ_BSTACK + 16 - 1 )  / 16;
	szBit += ( SZ_MSTACK + 16 - 1 )  / 16;
	szBit += ( REGSZ_L + 16 - 1 )  / 16;
	return szBit;
}

uint16 GetByteMapSize(  PLPLCSystem psys )
{
	uint16 szByte;
	szByte = psys->R[REG_T].sz;
	szByte += ( ( psys->R[REG_C].sz - DCOUNTER_ADDR ) << 1 ) + DCOUNTER_ADDR;
	szByte += psys->R[REG_D].sz;
	szByte += psys->R[REG_P].sz * 2;
	szByte += SZ_FSTACK * 2;
	return szByte;
}

api_res CreateMem( PLPLCSystem psys )
{
	psys->nBitMapsz = GetBitMapSize( psys );
	psys->nDataMapsz = GetByteMapSize( psys );
	psys->pBitMap = (uint16*)MEMNEW( psys->nBitMapsz * sizeof( uint16 ) );
	psys->pDataMap = (uint16*)MEMNEW( psys->nDataMapsz * sizeof( uint16 ) );
	if( psys->pBitMap != NULL && psys->pDataMap != NULL )
		return RES_OK;
	else
		return RES_ERR;
}

uint16* CreateBitMem( uint16 sz )
{
	uint16* p;
	sz = ( sz + 16 - 1 )  / 16;
	sz *= sizeof( uint16 );
	p = ( uint16* )MEMNEW( sz );
	memset( p, 0, sz );
	return p;
}
*/

api_res UpdateMem( PLPLCSystem psys, uint16* pByteBlock,int bytesz, uint16* pBitBlock,int bitsz )
{
	uint16 szX,szY,szM,szS,szT,szC,szD;
	//位元件
	psys->nBitMapsz = 0;
	szX = ( psys->R[REG_X].sz + 16 - 1 )  / 16;
	szY = ( psys->R[REG_Y].sz + 16 - 1 )  / 16;
	szM = ( psys->R[REG_M].sz + 16 - 1 )  / 16;
	szS = ( psys->R[REG_S].sz + 16 - 1 )  / 16;
	szT = ( psys->R[REG_T].sz + 16 - 1 )  / 16;
	szC = ( psys->R[REG_C].sz + 16 - 1 )  / 16;
	psys->nBitMapsz = ( szX + szY + szM + szS + szT + szC ) * 3;
	psys->nBitMapsz += ( SZ_BSTACK + 16 - 1 )  / 16;
	psys->nBitMapsz += ( SZ_MSTACK + 16 - 1 )  / 16;
	psys->nBitMapsz += ( REGSZ_L + 16 - 1 )  / 16;
	psys->nBitMapsz += ( SZ_FSTACK + 16 - 1 )  / 16;
	if( bitsz < psys->nBitMapsz )
		return RES_ERR;

	psys->nSysMAddr = psys->R[REG_M].sz - SZ_SYSREG_M;
	psys->pBitMap = pBitBlock;
	psys->R[REG_X].pState = psys->pBitMap;
	psys->R[REG_Y].pState = psys->R[REG_X].pState + szX;
	psys->R[REG_M].pState = psys->R[REG_Y].pState + szY;
	psys->R[REG_S].pState = psys->R[REG_M].pState + szM;
	psys->R[REG_T].pState = psys->R[REG_S].pState + szS;
	psys->R[REG_C].pState = psys->R[REG_T].pState + szT;
	
	psys->R[REG_X].pLastState = psys->R[REG_C].pState + szC;
	psys->R[REG_Y].pLastState = psys->R[REG_X].pLastState + szX;
	psys->R[REG_M].pLastState = psys->R[REG_Y].pLastState + szY;
	psys->R[REG_S].pLastState = psys->R[REG_M].pLastState + szM;
	psys->R[REG_T].pLastState = psys->R[REG_S].pLastState + szS;
	psys->R[REG_C].pLastState = psys->R[REG_T].pLastState + szT;

	psys->R[REG_X].pUpdate = psys->R[REG_C].pLastState + szC;
	psys->R[REG_Y].pUpdate = psys->R[REG_X].pUpdate + szX;
	psys->R[REG_M].pUpdate = psys->R[REG_Y].pUpdate + szY;
	psys->R[REG_S].pUpdate = psys->R[REG_M].pUpdate + szM;
	psys->R[REG_T].pUpdate = psys->R[REG_S].pUpdate + szS;
	psys->R[REG_C].pUpdate = psys->R[REG_T].pUpdate + szT;

	psys->R[REG_L].pState = psys->R[REG_C].pUpdate + szC;
	psys->R[REG_L].pData = 0;
	psys->R[REG_L].pLastState = 0;
	psys->R[REG_L].pUpdate = 0;

	psys->bStack.sz = SZ_BSTACK;
	psys->bStack.pBuf = psys->R[REG_L].pState + ( REGSZ_L + 16 - 1 )  / 16;
	psys->bStack.head = 0;
	psys->bStack.tail = -1;
	psys->mStack.sz = SZ_MSTACK;
	psys->mStack.pBuf = psys->bStack.pBuf + ( SZ_BSTACK + 16 - 1 )  / 16;
	psys->mStack.head = 0;
	psys->mStack.tail = -1;

	psys->bFStack.sz = SZ_FSTACK;
	psys->bFStack.pBuf = psys->bStack.pBuf + ( SZ_MSTACK + 16 - 1 )  / 16;
	psys->bFStack.head = 0;
	psys->bFStack.tail = -1;

	psys->R[REG_X].pData = 0;
	psys->R[REG_Y].pData = 0;
	psys->R[REG_M].pData = 0;
	psys->R[REG_S].pData = 0;

	//字元件
	szT = psys->R[REG_T].sz;
	szC = ( ( psys->R[REG_C].sz - DCOUNTER_ADDR ) * 2 ) + DCOUNTER_ADDR;
	szD = psys->R[REG_D].sz;
	psys->nDataMapsz = szT + szC + szD;
	psys->nDataMapsz += psys->R[REG_V].sz;
	psys->nDataMapsz += REGSZ_P * 2;
	psys->nDataMapsz += SZ_FSTACK * 2;
	//psys->nDataMapsz += SZ_BITCOIL_UPDATE * sizeof( BitCoilUpdate ) / 2;
	if( bytesz < psys->nDataMapsz )
		return RES_ERR;

	psys->nSysDAddr = psys->R[REG_D].sz - SZ_SYSREG_D;
	psys->pDataMap = pByteBlock;
	psys->R[REG_T].pData = psys->pDataMap;
	psys->R[REG_C].pData = psys->R[REG_T].pData + szT;
	psys->R[REG_D].pData = psys->R[REG_C].pData + szC;
	psys->R[REG_P].pData = psys->R[REG_D].pData + szD;
	psys->R[REG_V].pData = psys->R[REG_P].pData + psys->R[REG_P].sz * 2;
	psys->R[REG_D].pState = 0;
	psys->R[REG_P].pState = 0;
	psys->R[REG_V].pState = 0;
	psys->R[REG_D].pLastState = 0;
	psys->R[REG_P].pLastState = 0;
	psys->R[REG_V].pLastState = 0;
	psys->R[REG_D].pUpdate = 0;
	psys->R[REG_P].pUpdate = 0;
	psys->R[REG_V].pUpdate = 0;

	psys->pFStack.sz = SZ_FSTACK;
	psys->pFStack.pBuf = psys->R[REG_V].pData + psys->R[REG_V].sz;
	psys->pFStack.head = 0;
	psys->pFStack.tail = -1;

	//用于更新从串口设置的位元件
	//psys->pQueueCoilUpdate.pBuf = psys->pFStack.pBuf + SZ_FSTACK * 2;
	//psys->pQueueCoilUpdate.sz = SZ_BITCOIL_UPDATE;

	//测试
#if 0
	memset( psys->R[REG_X].pState,0x1,szX * sizeof( uint16 ) );
	memset( psys->R[REG_Y].pState,0x2,szY * sizeof( uint16 ) );
	memset( psys->R[REG_M].pState,0x3,szM * sizeof( uint16 ) );
	memset( psys->R[REG_S].pState,0x4,szS * sizeof( uint16 ) );
	memset( psys->R[REG_T].pState,0x5,szT * sizeof( uint16 ) );
	memset( psys->R[REG_C].pState,0x6,szC * sizeof( uint16 ) );

	memset( psys->R[REG_X].pLastState,0x11,szX * sizeof( uint16 ) );
	memset( psys->R[REG_Y].pLastState,0x12,szY * sizeof( uint16 ) );
	memset( psys->R[REG_M].pLastState,0x13,szM * sizeof( uint16 ) );
	memset( psys->R[REG_S].pLastState,0x14,szS * sizeof( uint16 ) );
	memset( psys->R[REG_T].pLastState,0x15,szT * sizeof( uint16 ) );
	memset( psys->R[REG_C].pLastState,0x16,szC * sizeof( uint16 ) );

	memset( psys->bStack.pBuf,0x1a,( SZ_BSTACK + 16 - 1 )  / 16 * sizeof( uint16 ) );
	memset( psys->mStack.pBuf,0x1b,( SZ_MSTACK + 16 - 1 )  / 16 * sizeof( uint16 ) );
	memset( psys->R[REG_L].pState,0x1c,( SZ_LSTATA + 16 - 1 )  / 16 * sizeof( uint16 ) );
	
	memset( psys->R[REG_T].pData,0xa1,psys->R[REG_T].sz * sizeof( uint16 ) );
	memset( psys->R[REG_C].pData,0xa2,psys->R[REG_C].sz * sizeof( uint16 ) );
	memset( psys->R[REG_C].pData + DCOUNTER_ADDR,0xb2,( ( psys->R[REG_C].sz - DCOUNTER_ADDR ) << 1 ) * sizeof( uint16 ) );
	memset( psys->R[REG_D].pData,0xa3,psys->R[REG_D].sz * sizeof( uint16 ) );
	memset( psys->R[REG_P].pData,0xa4,psys->R[REG_P].sz * sizeof( uint16 ) );
	_dDumpMem( psys );
#endif
	return RES_OK;
}

void tiny_mest( void* mem, int value, int sz )
{
	int i;
	for ( i = 0; i < sz; ++i )
		( (unsigned char*)mem )[i] = value;
}

void ZeroMem( PLPLCSystem psys )
{
	tiny_mest( psys->pBitMap, 0, psys->nBitMapsz );
	tiny_mest( psys->pDataMap, 0, psys->nDataMapsz );
}

api_res M_GetReg( mbyte** pp,LPRegGet pRegGet )
{
	pRegGet->pSource = *pp;
	pRegGet->iReg = **pp;
	++( *pp );
	if( pRegGet->iReg < REG_K )
	{
		pRegGet->addr = M_GetData16( *pp );
		M_SkipData16( *pp );
	}else if( pRegGet->iReg == REG_PV )
	{
		uint16 addr = M_GetData16( *pp );
		M_SkipData16( *pp );
		M_GetReg(  pp, pRegGet );
		pRegGet->addr += g_sys.R[REG_V].pData[addr];
	}else if( pRegGet->iReg < REG_DK )
	{
		pRegGet->udata16 = M_GetData16( *pp );
		M_SkipData16( *pp );
	}else
	{
		pRegGet->udata32 = M_GetData32( *pp );
		M_SkipData32( *pp );
	}
	return RES_OK;
}

api_res M_GetCoilData( enum eCoilDataType dataType, LPRegGet regGet, LPCoilData pcData )
{
	pcData->udata32 = 0;
	if( regGet->iReg < REG_K && g_sys.R[regGet->iReg].pData )
	{
		if( regGet->iReg == REG_D )
		{
			if( dataType == eUData16 )
			{
				pcData->udata16 = g_sys.R[regGet->iReg].pData[ regGet->addr ];
			}else
			{
				pcData->udata32 = *( (uint32*)&g_sys.R[ regGet->iReg ].pData[ regGet->addr ]);
			}
		}else if( regGet->iReg == REG_C && regGet->addr >= DCOUNTER_ADDR ){
			pcData->udata32 = *( (int32*)( g_sys.R[regGet->iReg].pData + DCOUNTER_ADDR ) + ( regGet->addr - DCOUNTER_ADDR ) );
		}else if( regGet->iReg == REG_P ){
			pcData->udata32 = g_sys.R[regGet->iReg].pData32[ regGet->addr ];
		}else{
			pcData->udata16 = g_sys.R[regGet->iReg].pData[ regGet->addr ];
		}
	}else if( regGet->iReg < REG_DK )
	{
		pcData->udata16 = regGet->udata16;
	}else
	{
		pcData->udata32 = regGet->udata32;
	}
	return RES_OK;
}

//第二次翻译，提高效率

api_res FormatIns( uint16** plcprg, mbyte** pFormat )
{
	*( (uint16*)*pFormat ) = **plcprg;
	M_SkipData16( *pFormat );
	++( *plcprg );
	return RES_OK;
}

api_res FormatCoil( uint16** plcprg, mbyte** pFormat )
{
	uint32 addr;
	mbyte nCoil;
	LPPLCCoil pCoil = (LPPLCCoil)*plcprg;
	mbyte sz = (mbyte)pCoil->cargc;

	switch( pCoil->ctype )
	{
	case 9:
		{
			nCoil = (mbyte)pCoil->coil;
			if( sz != 2 )
				return RES_ERR;
			addr = *( *plcprg + 1 );
			if( nCoil == REG_M )
			{
				if( addr >= 8000 )
					addr = g_sys.nSysMAddr + addr - 8000;
			}else if( nCoil == REG_D )
			{
				if( addr >= 8000 )
					addr = g_sys.nSysDAddr + addr - 8000;
			}
			if( addr >= g_sys.R[ nCoil ].sz )
				return RES_ERR;
		}
		break;
	case 0xf:
		if( pCoil->coil == 1 )
		{
			nCoil = REG_DK;
			addr = *( (uint32*)( *plcprg + 1 ) );
		}else
		{
			nCoil = REG_K;
			addr = *( *plcprg + 1 );
		}
		break;
	case 0xa:
		addr = *( *plcprg + 1 );
		if( pCoil->all == dcoilPV )
		{
			nCoil = REG_PV;
			if( addr >= g_sys.R[ REG_V ].sz )
				return RES_ERR;
		}
		break;
	default:
		return RES_ERR;
	}
	*( (*pFormat)++ ) = nCoil;
	if( nCoil == REG_DK )
	{
		*( (uint32*)( *pFormat ) ) = addr;
		M_SkipData32( *pFormat );
	}else{
		*( (uint16*)( *pFormat ) ) = addr;
		M_SkipData16( *pFormat );
	}
	*plcprg += sz;
	return RES_OK;
}

api_res FormatProgram( uint16** plcprg, mbyte** pFormat )
{
	if( M_IsPLCIns( *plcprg ) )
		return FormatIns( plcprg, pFormat );
	else
		return FormatCoil( plcprg, pFormat );
}

api_res M_LoadProgram( PLCSystem* psys )
{
	LPPLCIns pins;
	mbyte itype;
	mbyte bsz,msz;
	mbyte bMaxsz,mMaxsz,lMaxsz;
	uint16 addr;
	uint16* pPlcStep;		//
	uint16* pPlcPrgEnd;

	//检测需要的bStack，mStatck，以及需要保存状态的地方
	bsz = 0, bMaxsz = 0;
	msz = 0, mMaxsz = 0;
	lMaxsz = 0;
	pPlcPrgEnd = psys->pFlash + psys->szFlash;
	pPlcStep = psys->pFlash;
	psys->pPlcStep = psys->pPlcMain;

	while( pPlcStep < pPlcPrgEnd )
	{
		if( M_IsPLCIns( pPlcStep ) )
		{
			pins = (LPPLCIns)pPlcStep;
			itype = (mbyte)pins->itype;
			if( itype == 0 ) //etc. LD
				++bsz;
			else if( itype >= 0xa )//etc. OUT
			{
				bsz = 0;
			}
			if( pins->all == dinsMPS )
			{
				if( bMaxsz < bsz )
					bMaxsz = bsz;
				--bsz;
				++msz;
				if( mMaxsz < msz )
					mMaxsz = msz;
			}else if( pins->all == dinsMPP )
				--msz;
			else if( pins->all == dinsP )
			{
				++pPlcStep;
				addr = *( pPlcStep + 1 );
				pPlcStep += 2;
				if( psys->R[REG_P].pData32[addr] != REGP_INVALUED ) 
					return SetRunTimeError( eErrLoadPrg, 0, pPlcStep - psys->pFlash, 0 );
				psys->R[REG_P].pData32[addr] = psys->pPlcStep - psys->pPlcMain;
				continue;
			}else if( pins->all == dinsMEP //需要添加LState
				|| pins->all == dinsMEF 
				|| pins->all == dinsPLS 
				|| pins->all == dinsPLF
				|| pins->all == dinsCJP
				//|| pins->all == dinsOUTT
				|| pins->all == dinsOUTC 
				)
			{
				FormatProgram( &pPlcStep, &psys->pPlcStep );
				addr = lMaxsz++;
				*(psys->pPlcStep++) = REG_L;
				*( (uint16*)psys->pPlcStep ) = addr;
				psys->pPlcStep += 2;
				continue;
			}else if( pins->all == dinsEND )
			{
				FormatProgram( &pPlcStep, &psys->pPlcStep );
				break;
			}
		}
		if( FormatProgram( &pPlcStep, &psys->pPlcStep ) != RES_OK )
			return SetRunTimeError( eErrLoadPrg, 1, pPlcStep - psys->pFlash, 0 );
	}

	if( bMaxsz >= psys->bStack.sz )
		return SetRunTimeError( eErrLoadPrg, 2, pPlcStep - psys->pFlash, 0 );
	if( mMaxsz >= psys->mStack.sz )
		return SetRunTimeError( eErrLoadPrg, 3, pPlcStep - psys->pFlash, 0 );
	if( lMaxsz >= psys->R[REG_L].sz )
		return SetRunTimeError( eErrLoadPrg, 4, pPlcStep - psys->pFlash, 0 );

	M_dDump( psys->pPlcMain, psys->pPlcStep - psys->pPlcMain );
	return RES_OK;
}
