#include "PLCCore.h"
#include "MemManager.h"
#include "plc_instruction.h"

# pragma warning (disable:4996)

extern PLCSystem g_sys;

__inline mbyte _dIsIns( mbyte* pPlcStep )
{
	return !( *pPlcStep & 0xf000 );
}
__inline void _dSkip(  mbyte** pPlcStep )
{
	if( **pPlcStep == REG_DK )
		*pPlcStep += 3;
	else if( **pPlcStep < REG_COUNT || **pPlcStep == REG_K)
		*pPlcStep += 2;
	else
		++( *pPlcStep );
}
__inline void _dSkipIns( mbyte** pPlcStep )
{
	++( *pPlcStep );
}
__inline void _dSkipArg( mbyte** pPlcStep )
{
	if( **pPlcStep == REG_DK )
		*pPlcStep += 3;
	else
		*pPlcStep += 2;
}
void _dDumpMem( PLPLCSystem psys )
{
#ifdef _WIN32
	FILE* fb;
	fb = fopen( "mem.dump","wb" );
	if( fb == NULL )
		return;
	fwrite( psys->pBitMap, sizeof(uint16), psys->nBitMapsz, fb );
	fwrite( psys->pDataMap, sizeof(uint16), psys->nDataMapsz, fb );
	fwrite( psys->R[REG_D].pData, sizeof( uint16 ),psys->R[REG_D].sz, fb );
	fclose( fb );
#endif
}
void _dDump( mbyte* pdata, uint16 sz )
{
#ifdef _WIN32
	FILE* fb;
	fb = fopen( "program.dump","wb" );
	if( fb == NULL )
		return;
	fwrite( pdata, sizeof(mbyte), sz, fb );
	fclose( fb );
#endif
}

api_res dGetFormatCoil( mbyte nCoil, uint16 addr, LPReg* ppCoil, uint16* paddr )
{
	//if( nCoil == dcoilDT ) nCoil = dcoilT;
	//else if( nCoil == dcoilDC ) nCoil = dcoilC;
	nCoil -= 0x90;
	if( nCoil < REG_COUNT )
	{
		*ppCoil = &g_sys.R[ nCoil ];
		*paddr = addr;
		if( nCoil == REG_M )
		{
			if( *paddr >= 8000 )
				*paddr = g_sys.nSysMAddr + addr - 8000;
		}else if( nCoil == REG_D )
		{
			if( *paddr >= 8000 )
				*paddr = g_sys.nSysDAddr + addr - 8000;
		}else if( nCoil == REG_C )
		{

		}
		if( *paddr >= (*ppCoil)->sz )
			return RES_ERR;
	}else 
		return RES_ERR;
	return RES_OK;
}


//只有两种数据16位或者32位，即sz只能是1或者2
api_res StrReadData( mbyte* strCoil,uint16* pData, uint16 sz )
{
	uint16 n;
	uint16* pMem;

	if( *strCoil < REG_COUNT )
	{
		pMem = g_sys.R[ *strCoil ].pData + *( strCoil + 1);
		if( pMem == NULL )
			return RES_ERR;
		n = sz;
	}else if( *strCoil == REG_K )
	{
		pMem = strCoil + 1;
		n = 1;
	}else if( *strCoil == REG_DK )
	{
		pMem = strCoil + 1;
		n = 2;
	}else
		return RES_ERR;
	if( sz == 1 )
	{
		*pData = *pMem;
	}else if( sz == 2 )
	{
		if( n == 1 )
		{
			if( *strCoil == REG_K )//10进制常数带符号
				*((int32*)pData) = *((int16*)pMem);
			else//16进制不带符号
				*((uint32*)pData) = *pMem;
		}
		else
			*((uint32*)pData) = *((uint32*)pMem);
	}else
		return RES_ERR;
	return RES_OK;
}

//第二次翻译，提高效率
api_res _dFormatIns( mbyte** plcprg, mbyte** pFormat )
{
	*( (*pFormat)++ ) = *( (*plcprg)++ );
	return RES_OK;
}
api_res _dFormatCoil( mbyte** plcprg, mbyte** pFormat )
{
	uint16 addr;
	uint16 nCoil = **plcprg & 0x0fff;
	mbyte sz = ( **plcprg & 0xf000 ) >> 12;

	if( nCoil < dcoilK )
	{
		if( sz != 2 )
			return RES_ERR;
		nCoil -= dcoilX;
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
		*( (*pFormat)++ ) = nCoil;
		*( (*pFormat)++ ) = addr;
	}else if( nCoil == dcoilK )
	{
		*( (*pFormat)++ ) = REG_K;
		*( (*pFormat)++ ) = *( *plcprg + 1 );
	}else if( nCoil == dcoilDK )
	{
		*( (*pFormat)++ ) = REG_DK;
		*( (*pFormat)++ ) = *( *plcprg + 1 );
		*( (*pFormat)++ ) = *( *plcprg + 2 );
	}else
		return RES_ERR;
	*plcprg += sz;
	return RES_OK;
}
api_res dFormatProgram( mbyte** plcprg, mbyte** pFormat )
{
	if( _dIsIns( *plcprg ) )
		return _dFormatIns( plcprg, pFormat );
	else
		return _dFormatCoil( plcprg, pFormat );
}

api_res API_LoadProgram()
{
	mbyte ins;
	uint16 bsz,msz;
	uint16 bMaxsz,mMaxsz,sMaxsz;
	uint16 addr;
	mbyte* pPlcStep;		//
	mbyte* pPlcPrgEnd;

	//ResetMem();
	ResetTimerList();

	//function stack
	for ( addr = 0; addr < REGSZ_P; ++addr )
		g_sys.R[REG_P].pData[ addr ] = 0xffff;

	//检测需要的bStack，mStatck，以及需要保存状态的地方
	bsz = 0, bMaxsz = 0;
	msz = 0, mMaxsz = 0;
	sMaxsz = 0;
	pPlcPrgEnd = g_sys.pFlash + g_sys.szFlash;
	pPlcStep = g_sys.pFlash;
	g_sys.pPlcStep = g_sys.pPlcMain;

	while( pPlcStep < pPlcPrgEnd )
	{
		if( _dIsIns( pPlcStep ) )
		{
			ins = *pPlcStep & 0x0fff;
			if( ins <= dinsLDF )
				++bsz;
			else if( ins == dinsOUT || ins == dinsOUTT || dinsOUTC == ins )
			{
				if( bMaxsz < bsz )
					bMaxsz = bsz;
				bsz = 0;
				if( dinsOUTT == ins || dinsOUTC == ins )
				{
					dFormatProgram( &pPlcStep, &g_sys.pPlcStep );
					*(g_sys.pPlcStep++) = sMaxsz++;
				}
			}else if( ins == dinsMPS )
			{
				++msz;
				if( mMaxsz < msz )
					mMaxsz = msz;
			}else if( ins == dinsMPP )
				--msz;
			else if( ins == dinsP )
			{
				//dFormatProgram( &g_sys.pPlcStep, &pRam );
				++pPlcStep;
				addr = *( pPlcStep + 1 );
				pPlcStep += 2;
				if( g_sys.R[REG_P].pData[ addr ] != 0xffff ) 
					return RES_ERR;
				g_sys.R[REG_P].pData[ addr ] = g_sys.pPlcStep - g_sys.pPlcMain;
				continue;
			}else if( ins == dinsMEP 
				|| ins == dinsMEF 
				|| ins == dinsPLS 
				|| ins == dinsPLF
				|| ins == dinsCJP )
			{
				dFormatProgram( &pPlcStep, &g_sys.pPlcStep );
				*(g_sys.pPlcStep++) = sMaxsz++;
			}else if( ins == dinsEND )
			{
				dFormatProgram( &pPlcStep, &g_sys.pPlcStep );
				break;
			}
		}
		if( dFormatProgram( &pPlcStep, &g_sys.pPlcStep ) != RES_OK )
			return RES_ERR;
	}
	if( bMaxsz >= g_sys.bStack.sz )
		return RES_ERR;
	if( mMaxsz >= g_sys.mStack.sz )
		return RES_ERR;
	if( sMaxsz >= g_sys.regLS.sz )
		return RES_ERR;
	
	CLRBIT( g_sys.R[ REG_M ].pLastState,SYSADDRM_INITP );
	SETBIT( g_sys.R[ REG_M ].pState,SYSADDRM_INITP );

	//_dDump( g_sys.pPlcMain, g_sys.pPlcStep - g_sys.pPlcMain );

	g_sys.bMatherLine = 0;
	g_sys.pPlcStep = g_sys.pPlcMain;
	g_sys.pStl = 0;
	g_sys.nTimerCount = 0;

	return RES_OK;
}


__inline void SkipSTL( mbyte** ppStep )
{
	while( **ppStep != dinsRET && **ppStep != dinsSTL )
		_dSkip( ppStep );
}

__inline void ClearSTL( mbyte* pStep )
{
	LPReg pCoil; 
	uint16 addr;
	addr = *(pStep - 1);
	UpdateStata( &g_sys.R[REG_S],addr, 0 );
	while( *pStep != dinsRET && *pStep != dinsSTL )
	{
		if( *pStep == dinsOUT )
		{
			_dSkipIns( &pStep );
			pCoil = &g_sys.R[ MBYTE2REG( *pStep ) ];
			addr = *( pStep + 1 );
			UpdateStata( pCoil, addr,0 );
		}
		_dSkip( &pStep );
	}
}

api_res API_PLCScan()
{
	mbyte b;
	mbyte ins;
	uint16 insType;
	LPReg pCoil; 
	uint16 addr;
	uint16 uData16;
	int32 Data32;
	uint16* pCounter1;
	int32* pCounter2;
	uint16 uArg1,uArg2;

	do
	{
		ins = *g_sys.pPlcStep;
		insType = *g_sys.pPlcStep & 0xff00;
		if( insType == 0x0000 )
		{
			switch( ins & 0x00f0 )
			{
			case 0x10:
				{
					_dSkipIns( &g_sys.pPlcStep );
					pCoil = &g_sys.R[ MBYTE2REG( *g_sys.pPlcStep ) ];
					addr = *( g_sys.pPlcStep + 1 );
					b = GETBIT( pCoil->pState,addr );
					if( ins <= dinsLDF )
					{
						if( ins == dinsLDF )
							b = ( !b && GETBIT( pCoil->pLastState,addr ));
						else if( ins == dinsLDP )
							b = ( b && !GETBIT( pCoil->pLastState,addr ));
						else if( ins == dinsLDI )
							b = !b;
						BitStackPush( &g_sys.bStack, g_sys.bMatherLine );
						g_sys.bMatherLine = b;
					}else if( ins <= dinsORF )
					{
						if( ins == dinsORI )
							b = !b;
						else if( ins == dinsORF )
							b = ( !b && GETBIT( pCoil->pLastState,addr ));
						else if( ins == dinsORP )
							b = ( b && !GETBIT( pCoil->pLastState,addr ));
						g_sys.bMatherLine |= b;
					}else if( ins <= dinsANDF )
					{
						if( ins == dinsANI )
							b = !b;
						else if( ins == dinsANDF )
							b = ( !b && GETBIT( pCoil->pLastState,addr ));
						else if( ins == dinsANDP )
							b = ( b && !GETBIT( pCoil->pLastState,addr ));
						g_sys.bMatherLine &= b;
					}else
						return RES_ERR;
				}
				break;
			case 0x20:
				{
					if( ins <= dinsANB )
					{
						b = BitStackRead( &g_sys.bStack );
						BitStackPop( &g_sys.bStack );
						if( ins == dinsANB )
							g_sys.bMatherLine &= b;
						else
							g_sys.bMatherLine |= b;
					}else if( ins == dinsMPS )
					{
						BitStackPush( &g_sys.mStack, g_sys.bMatherLine );
					}else if( ins <= dinsMPP )
					{
						g_sys.bMatherLine = BitStackRead( &g_sys.mStack );
						if( ins == dinsMPP ) BitStackPop( &g_sys.mStack );
					}else if( ins == dinsINV )
					{
						g_sys.bMatherLine = !g_sys.bMatherLine;
					}else if( ins <= dinsMEP )
					{
						uArg1 = *( g_sys.pPlcStep++ );
						pCoil = &g_sys.R[ MBYTE2REG( *g_sys.pPlcStep ) ];
						addr = *( g_sys.pPlcStep + 1 );
						b = GETBIT(  g_sys.regLS.pState, uArg1 );
						if( ins == dinsMEP )
							b = ( !b && g_sys.bMatherLine );
						else
							b = ( b && !g_sys.bMatherLine );
						UpdateStata( pCoil,addr, b );
						b ? SETBIT( g_sys.regLS.pState, uArg1 ) : CLRBIT( g_sys.regLS.pState, uArg1 );
					}
				}
				break;
			case 0x30:
				{
					_dSkipIns( &g_sys.pPlcStep );
					if( ins <= dinsRSTT )
					{
						pCoil = &g_sys.R[ MBYTE2REG( *g_sys.pPlcStep ) ];
						addr = *( g_sys.pPlcStep + 1 );
						if( ins == dinsSET )
						{
							if( g_sys.bMatherLine )
								UpdateStata( pCoil, addr,g_sys.bMatherLine );
						}else if( ins == dinsRST )
						{
							if( g_sys.bMatherLine )
								UpdateStata( pCoil, addr,0 );
						}else if(  ins == dinsOUT ) 
							UpdateStata( pCoil, addr,g_sys.bMatherLine );
						else if(  ins == dinsRSTT ) 
						{
							if( g_sys.bMatherLine )
							{
								pCoil->pData[addr] = 0;
								UpdateStata( pCoil, addr,0 );
							}
						}else if(  ins == dinsRSTC ) 
						{
							if( g_sys.bMatherLine )
							{
								if( addr < DCOUNTER_ADDR ) //单向计数
								{
									pCounter1 = pCoil->pData + addr;
									( *pCounter1 ) = 0;
									UpdateStata( pCoil, addr,0 );
								}else //双向计数
								{
									pCounter2 = (int32*)( pCoil->pData + addr + ( ( addr - 200 ) << 1 ) );
									( *pCounter2 ) = 0;
									UpdateStata( pCoil, addr,0 );
								}
							}
						}
						BitStackPop( &g_sys.bStack );
					}else if( ins <= dinsPLF )
					{
						uArg1 = *( g_sys.pPlcStep++ );
						pCoil = &g_sys.R[ MBYTE2REG( *g_sys.pPlcStep ) ];
						addr = *( g_sys.pPlcStep + 1 );
						_dSkipArg( &g_sys.pPlcStep );
						b = GETBIT( pCoil->pState, addr );
						if( ins == dinsPLS )
						{
							if( g_sys.bMatherLine && !GETBIT( g_sys.regLS.pState, uArg1 ) )
								g_sys.fncCallBack.pSetOutPut( addr, 1 );
							UpdateStata( pCoil, addr,0 );
						}
						else if( ins == dinsPLF )
						{
							if( !g_sys.bMatherLine && GETBIT( g_sys.regLS.pState, uArg1 ) )
								g_sys.fncCallBack.pSetOutPut( addr, 1 );
							UpdateStata( pCoil, addr,0 );
						}
						else if( ins == dinsOUTT )
						{
							if( pCoil != &g_sys.R[REG_T] )
								return RES_ERR;
							if( g_sys.bMatherLine )
							{
								if( !GETBIT(  g_sys.regLS.pState, uArg1 ) && !b )
									AddToTimerList( addr,g_sys.pPlcStep );
							}
							else
							{
								if( GETBIT( g_sys.regLS.pState, uArg1 ) )
									ReMoveTimerNod( addr );
								if( b )
									UpdateStata( pCoil, addr,g_sys.bMatherLine );
							}
						}else if ( ins == dinsOUTC )
						{
							if( pCoil != &g_sys.R[REG_C] )
								return RES_ERR;
							if( addr < DCOUNTER_ADDR ) //单向计数
							{
								pCounter1 = pCoil->pData + addr;
								if( g_sys.bMatherLine && !b && !GETBIT(  g_sys.regLS.pState, uArg1 ) )
								{
									( *pCounter1 )++;
									uData16 = 0;
									StrReadData( g_sys.pPlcStep,&uData16,1 );
									if( *pCounter1 >= uData16 )
										UpdateStata( pCoil, addr,1 );
								}
							}else //双向计数
							{
								pCounter2 = (int32*)( pCoil->pData + addr + ( ( addr - 200 ) << 1 ) );
								if( g_sys.bMatherLine && !b && !GETBIT(  g_sys.regLS.pState, uArg1 ) )
								{
									Data32 = 0;
									StrReadData( g_sys.pPlcStep,(uint16*)&Data32,2 );
									if( GETBIT( g_sys.R[ REG_M ].pState, SYSADDR_OFFSETM + addr ) )
									{
										( *pCounter2 )--;
										b = ( *pCounter2 <= Data32 );
									}else
									{
										( *pCounter2 )++;
										b = ( *pCounter2 >= Data32 );
									}
									if( b )
										UpdateStata( pCoil, addr,1 );
								}
							}
						}
						g_sys.bMatherLine ? SETBIT( g_sys.regLS.pState, uArg1 ) : CLRBIT( g_sys.regLS.pState, uArg1 );
					}
				}
				break;
			case 0x40:
				if( ins == dinsEND )
				{
					//结束本次扫描
					return UpdateLastStata();
					//g_sys.fncCallBack.pScanEnd();
				}else
					return RES_ERR;
				break;
			case 0x50:
				{
					if( ins == dinsSTL )
					{
						_dSkipIns( &g_sys.pPlcStep );
						if( !GETBIT( g_sys.R[REG_S].pState, *( g_sys.pPlcStep + 1 ) ) )
						{
							SkipSTL( &g_sys.pPlcStep );
							g_sys.pStl = NULL;
						}else
						{
							_dSkipArg( &g_sys.pPlcStep );
							g_sys.pStl = g_sys.pPlcStep;
							g_sys.bMatherLine = 1;
						}
						continue;
					}else if( ins == dinsRET )
					{
						g_sys.pStl = NULL;
					}else if( ins == dinsSET1 )
					{
						_dSkipIns( &g_sys.pPlcStep );
						if( g_sys.bMatherLine )
						{
							addr = *( g_sys.pPlcStep + 1 );
							UpdateStata( &g_sys.R[REG_S],addr, 1 );
							if( g_sys.pStl )//在stl里面set
								ClearSTL( g_sys.pStl );
						}
					}else if( ins == dinsRST1 )
					{
						_dSkipIns( &g_sys.pPlcStep );
						if( g_sys.bMatherLine )
						{
							addr = *( g_sys.pPlcStep + 1 );
							UpdateStata( &g_sys.R[REG_S],addr, 0 );
							if( g_sys.pStl && *( g_sys.pStl - 1 ) == addr )
								ClearSTL( g_sys.pStl );
						}
					}
				}
				break;
			default:
				return RES_ERR;
			}
		}else if( insType <= 0x3000 )//应用指令
		{
			_dSkipIns( &g_sys.pPlcStep );
			if( ins <= dinsDIV )
			{
				StrReadData( g_sys.pPlcStep,&uArg1,1 );
				_dSkipArg( &g_sys.pPlcStep );
				StrReadData( g_sys.pPlcStep,&uArg2,1 );
				_dSkipArg( &g_sys.pPlcStep );
				pCoil = &g_sys.R[ MBYTE2REG( *g_sys.pPlcStep ) ];
				addr = *( g_sys.pPlcStep + 1 );
				if( g_sys.bMatherLine )
				{
					if( ins == dinsDIV )
						pCoil->pData[addr] = uArg1 / uArg2;
					else if( ins == dinsADD )
						pCoil->pData[addr] = uArg1 + uArg2;
					else if( ins == dinsSUB )
						pCoil->pData[addr] = uArg1 - uArg2;
					else if( ins == dinsMUL )
						pCoil->pData[addr] = uArg1 * uArg2;
				}
			}else if( ins <= dinsDEC )
			{
				pCoil = &g_sys.R[ MBYTE2REG( *g_sys.pPlcStep ) ];
				addr = *( g_sys.pPlcStep + 1 );
				if( g_sys.bMatherLine )
				{
					if( ins == dinsDEC )
						--pCoil->pData[addr];
					else
						++pCoil->pData[addr];
				}
			}else if( ins == dinsMOV )
			{
				StrReadData( g_sys.pPlcStep,&uArg1,1 );
				_dSkipArg( &g_sys.pPlcStep );
				pCoil = &g_sys.R[ MBYTE2REG( *g_sys.pPlcStep ) ];
				addr = *( g_sys.pPlcStep + 1 );
				if( g_sys.bMatherLine ) 
					pCoil->pData[addr] = uArg1;
			}else if( ins == dinsCMP )
			{
				StrReadData( g_sys.pPlcStep,&uArg1,1 );
				_dSkipArg( &g_sys.pPlcStep );
				StrReadData( g_sys.pPlcStep,&uArg2,1 );
				_dSkipArg( &g_sys.pPlcStep );
				pCoil = &g_sys.R[ MBYTE2REG( *g_sys.pPlcStep ) ];
				addr = *( g_sys.pPlcStep + 1 );
				if( g_sys.bMatherLine )
				{
					if( uArg1 > uArg2 )
						UpdateStata( pCoil,addr, 1 );
					else if( uArg1 == uArg2 )
						UpdateStata( pCoil,addr + 1, 1 );
					else
						UpdateStata( pCoil,addr + 2, 1 );
				}
			}else if( ins == dinsSRET )
			{
				uData16 = StackRead( &g_sys.pFStack );
				StackPop( &g_sys.pFStack );
				g_sys.pPlcStep = g_sys.pPlcMain + uData16;
				continue;
			}else if( ins == dinsFEND )
			{
				//结束本次扫描
				return UpdateLastStata();
				//g_sys.fncCallBack.pScanEnd();
			}else if( ins <= dinsCJP )
			{
				if( ins == dinsCJP )
				{
					uArg1 = *( g_sys.pPlcStep++ );
					b = GETBIT( g_sys.regLS.pState, uArg1 );
					if( b != g_sys.bMatherLine )
						g_sys.bMatherLine ? SETBIT( g_sys.regLS.pState, uArg1 ) : CLRBIT( g_sys.regLS.pState, uArg1 );
					b = ( g_sys.bMatherLine && !b );
				}else
					b = g_sys.bMatherLine;
				if( b )
				{
					addr = *( g_sys.pPlcStep + 1 );
					if( ins == dinsCALL )
					{
						_dSkip( &g_sys.pPlcStep );
						StackPush( &g_sys.pFStack,g_sys.pPlcStep - g_sys.pPlcMain );
					}
					g_sys.pPlcStep = g_sys.pPlcMain + g_sys.R[REG_P].pData[ addr ];
					continue;
				}
			}else if( ins == dinsP )
			{
				//现在的版本不可能运行到这里
				//_dSkipArg( &g_sys.pPlcStep );
			}
		}else
			return RES_ERR;
		_dSkip( &g_sys.pPlcStep );
	} while ( g_sys.pPlcStep < g_sys.pPlcScanPos );
	return RES_OK;
}

#pragma warning(default:4996)
