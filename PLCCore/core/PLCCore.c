/*
 *	第三个版本，解析运行
 *	将设备相关分离开来
 */
//core 第二版本 使用内存块

#include "PLCCore.h"
#include "MemManager.h"
#include "insClass.h"

PLCSystem g_sys = {0};

/*
 *	DSP资源
 */

#define GETBITMAPSIZE( sz ) ( ( sz + 16 - 1 )  / 16 )

//这里注意大小，如果更改了REG的大小后这里也要相应修改
//第二个版本，不用修改了
#define BITMAP_SZ \
( ( GETBITMAPSIZE( REGSZ_X )\
 +GETBITMAPSIZE( REGSZ_Y )\
 +GETBITMAPSIZE( REGSZ_M )\
 +GETBITMAPSIZE( REGSZ_S )\
 +GETBITMAPSIZE( REGSZ_T )\
 +GETBITMAPSIZE( REGSZ_C ) ) * 3\
+GETBITMAPSIZE( SZ_BSTACK )\
+GETBITMAPSIZE( SZ_MSTACK )\
+GETBITMAPSIZE( REGSZ_L )\
+GETBITMAPSIZE( SZ_FSTACK ) )

#define BYTEMAP_SZ \
( REGSZ_T + ( REGSZ_C - DCOUNTER_ADDR ) * 2 + DCOUNTER_ADDR\
+ REGSZ_D + REGSZ_V + REGSZ_P * 2\
+ SZ_FSTACK * 2 )//\
//+ ( SZ_BITCOIL_UPDATE * sizeof( BitCoilUpdate ) / 2 ) )

//#pragma DATA_SECTION(d,"TestData");
//#pragma DATA_SECTION(d,"TestData");
uint16 g_pBitMap[ BITMAP_SZ ];
uint16 g_pByteMap[ BYTEMAP_SZ ];


PLPLCSystem GetSystem( void ){ return &g_sys; }

//定时器列表
LPStaticLink MallcoTimerNod( void )
{
	LPStaticLink pNod = g_sys.sLinkTimer;
	LPStaticLink pNodEnd = g_sys.sLinkTimer + REGSZ_T;
	while ( pNod < pNodEnd )
	{
		if( pNod->id == ENTIMER_INVALUED )
			return pNod;
		++pNod;
	}
	return NULL;
}
__inline void FreeTimerNod( LPStaticLink pNod )
{
	pNod->id = ENTIMER_INVALUED;
}

api_res AddToTimerList( mbyte id,mbyte* pTimeOut )
{
	uint16* pCounter;
	LPStaticLink pNod;
	LPStaticLink* ppNod = NULL;
	if( GETBIT(  g_sys.R[ REG_L ].pState, id + TWORK_ADDR ) )
		return RES_OK;
	SETBIT(  g_sys.R[ REG_L ].pState, id + TWORK_ADDR );
	if( id < 200 )	//100ms
		ppNod = &g_sys.pEnT100ms;
	else if( id < 246 )//10ms
		ppNod = &g_sys.pEnT10ms;
	else if( id < 250 )//1ms 保持
		ppNod = &g_sys.pEnT1ms;
	else if( id < 256 )//100ms 保持
		ppNod = &g_sys.pEnT100ms;
	if( ppNod == NULL )
		return RES_ERR;
	pCounter = g_sys.R[REG_T].pData + id;
	pNod = *ppNod;
	while( pNod != NULL )
	{
		if( pNod->id == id )
			break;
		pNod = pNod->pNext;
	}
	if( pNod == NULL )
	{
		pNod = MallcoTimerNod();
		if( pNod == NULL )	return RES_ERR;
		pNod->pCounter = pCounter;
		pNod->id = id;
		pNod->pNext = *ppNod;
		pNod->pTimeOut = pTimeOut;
		*ppNod = pNod;
	}
	return RES_OK;
}
void ReMoveTimerNod( mbyte id )
{
	mbyte bClear = 1;
	uint16* pCounter;
	LPStaticLink* ppNod = NULL;
	LPStaticLink pNod;
	if( !GETBIT(  g_sys.R[ REG_L ].pState, id + TWORK_ADDR ) )
		return;
	CLRBIT(  g_sys.R[ REG_L ].pState, id + TWORK_ADDR );
	if( id < 200 )	//100ms
		ppNod = &g_sys.pEnT100ms;
	else if( id < 246 )//10ms
		ppNod = &g_sys.pEnT10ms;
	else if( id < 250 )//1ms 保持
		ppNod = &g_sys.pEnT1ms, bClear = 0;
	else if( id < 256 )//100ms 保持
		ppNod = &g_sys.pEnT100ms, bClear = 0;
	if( ppNod == NULL )
		return;
	pCounter = g_sys.R[REG_T].pData + id;
	while( *ppNod != NULL )
	{
		if( (*ppNod)->id == id )
		{
			if( bClear )
				*pCounter = 0;
			pNod = *ppNod;
			*ppNod = ( *ppNod )->pNext;
			FreeTimerNod( pNod );
			break;
		}
		ppNod = &( *ppNod )->pNext;
	}
}

void ResetTimerList( void )
{
	LPStaticLink pNod = g_sys.sLinkTimer;
	LPStaticLink pNodEnd = g_sys.sLinkTimer + REGSZ_T;
	while ( pNod < pNodEnd )
	{
		pNod->id = ENTIMER_INVALUED;
		++pNod;
	}
	g_sys.pEnT100ms = NULL;
	g_sys.pEnT10ms = NULL;
	g_sys.pEnT1ms = NULL;
}

void UpdateRegBlocksz( void )
{
	uint16 i;
	LPReg pr = g_sys.R;
	for( i = 0; i <= REG_UPDATE; ++i,++pr )
	{
		pr->uBlocksz = 1;
		while( ( pr->uBlocksz * 16 * 32 ) < pr->sz )
			pr->uBlocksz++;
	}
}

void WriteBitCoil( LPReg preg, uint16 addr, mbyte bs )
{
	//uint16 bsz;
	GETBIT( preg->pState, addr ) ? SETBIT( preg->pLastState, addr ) : CLRBIT( preg->pLastState, addr );
	bs ? SETBIT( preg->pState, addr ) : CLRBIT( preg->pState, addr );
	CLRBIT( preg->pUpdate, addr );
	/*if( bs != GETBIT( preg->pLastState, addr ) )
	{
		if( preg->uBlocksz )
		{
			bsz = preg->uBlocksz << 4;
			addr = addr / bsz;
			SETBIT( (uint16*)&preg->uUpdateFlag,addr );
		}
	}*/
}

void UpdateBitCoil( LPReg preg, uint16 addr )
{
	uint16 bsz;
	if( GETBIT( preg->pState, addr ) != GETBIT( preg->pLastState, addr ) )
	{
		SETBIT( preg->pUpdate, addr );
		if( preg->uBlocksz )
		{
			bsz = preg->uBlocksz << 4;
			addr = addr / bsz;
			SETBIT( (uint16*)&preg->uUpdateFlag,addr );
		}
	}
}

api_res UpdateLastStata( void )
{
	uint16 addr;
	uint16 i,j,k,u,c;
	uint16* pu;
	LPReg pr;
	
	for( i = 0; i <= REG_UPDATE; i++ )
	{
		pr = &g_sys.R[ i ];
		if( pr->uUpdateFlag )
		{
			j = 0;
			addr = 0;
			u = 0;
			while( pr->uUpdateFlag && j < 32 )
			{
				if( GETBIT( ( (uint16*)&pr->uUpdateFlag ),j ) )
				{
					c = 0;
					pu = pr->pUpdate + u;
					while( c < pr->uBlocksz )
					{
						k = addr + c * 16;
						while( *pu )
						{
							if( GETBIT( pr->pUpdate, k ) )
							{
								GETBIT( pr->pState, k ) ? SETBIT( pr->pLastState, k ) : CLRBIT( pr->pLastState, k );
								CLRBIT( pr->pUpdate, k );
							}
							++k;
						}
						++pu;
						++c;
					}
					
					CLRBIT( (uint16*)&pr->uUpdateFlag,j );
				}
				u += pr->uBlocksz;
				addr += 16;
				j++;
			}
			//测?出错 实际运行除这行
			//if( j > 32 )
			//	j++;
		}
	}
	return RES_OK;
}


__inline void SetScanPos( mbyte* pPos )
{
	g_sys.pPlcScanPos = pPos;
}



api_res GetFormatCoil( mbyte nCoil, uint16 addr, LPReg* ppCoil, uint16* paddr )
{
	//if( nCoil == api_coilDT ) nCoil = api_coilT;
	//else if( nCoil == api_coilDC ) nCoil = api_coilC;
	if( nCoil >= 0xa0 ) nCoil -= 0x10;
	nCoil -= 0x90;
	if( nCoil < REG_COUNT )
	{
		*ppCoil = g_sys.R + nCoil;
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


void PLCScanEnd( void )
{
	LPReg pReg;
	mbyte senBuf[ MAX_PROTOCL_BUF_SZ ];
	uint16 szMonitor;
	uint16* pMonotor;
	mbyte* pBuf;
	uint16* pMRegStata = ( g_sys.R + REG_M )->pState ;
	uint16 addr;
	uint16 szRead;
	PLCCoil coil;

	//重置PLC
	g_sys.pPlcStep = g_sys.pPlcMain;
	g_sys.bMatherLine = 0;
	//检测有没有出错
	if( g_sys.bStack.tail != -1 || g_sys.mStack.tail != -1 )
	{
		SetRunTimeError( eErrScanEndCheck,0, 0, 0 );
		return;
	}

	API_FeedWDG();
	//scan time
	g_sys.R[ REG_D ].pData[ SYSADDRD_SCANTIME ] = g_sys.uScanTime;
	g_sys.uScanTime = 0;

	if( GETBIT( pMRegStata,SYSADDRM_MONITORSTART ) )
	//while(1)
	{
		CLRBIT( pMRegStata,SYSADDRM_MONITORSTART );
		szMonitor = ( g_sys.R + REG_D )->pData[SYSADDRD_MONITORLEN];
		pMonotor = ( g_sys.R + REG_D )->pData + SYSADDRD_MONITORSTART;
		if( szMonitor > SYSADDRD_MONITORLEN - SYSADDRD_MONITORSTART )
			szMonitor = SYSADDRD_MONITORLEN - SYSADDRD_MONITORSTART;
		//senBuf[0] = CMD_WRITE;
		pBuf = senBuf;
		*pBuf++ = CMD_WRITE;
		*pBuf++ = api_coilD & 0x00ff;
		*pBuf++ = ( ( 8040 & 0xff00 ) >> 8 );
		*pBuf++ = ( 8040 & 0x00ff);
		*pBuf++ = 0;
		while( szMonitor-- )
		{
			addr = *pMonotor++;
			if( ( addr & 0x8000 ) == 0 )
			{
				szRead = 0;
				//pBuf = senBuf + 1;
				if( addr < 500 )
					coil.all = api_coilX;
				else if( addr < 1000 )
					coil.all = api_coilY,addr -= 500;
				else if( addr < 1500 )
					coil.all = api_coilT,addr -= 1000;
				else if( addr < 2000 )
					coil.all = api_coilC,addr -= 1500;
				else if( addr < 11000 )
					coil.all = api_coilM,addr -= 2000;
				else if( addr < 20000 )
					coil.all = api_coilS,addr -= 11000;
				else if( addr < 20500 )
					coil.all = api_coilDT,szRead = 1,addr -= 20000;
				else if( addr < 21000 )
					coil.all = api_coilDC,szRead = 1,addr -= 20500;
				else if( addr < 30000 )
					coil.all = api_coilD,szRead = 1,addr -= 21000;
				else if( addr < 30500 )
					coil.all = api_coilDDT,szRead = 2,addr -= 30000;
				else
					continue;
				if( GetFormatCoil( coil.all & 0x00ff, addr, &pReg, &addr ) != RES_OK )
					continue;
				//++pBuf;
				//*pBuf++ = ( ( addr & 0xff00 ) >> 8 );
				//*pBuf++ = ( addr & 0x00ff);
				senBuf[4] += ( ( szRead == 0 ) ? 1 : szRead );
				if( szRead == 0 )
				{
					*pBuf++ = 0;
					if( pReg->pState )
						*pBuf++ = GETBIT( pReg->pState, addr );
					else
						continue;
				}else if( szRead == 1 )
				{
					if( pReg->pData )
					{
						szRead = *( pReg->pData + addr );
						*pBuf++ = ( ( szRead & 0xff00 ) >> 8 );
						*pBuf++ = ( szRead & 0x00ff);
					}else
						continue;
				}else if ( szRead == 2 )
				{
					if( pReg->pData )
					{
						szRead = *( pReg->pData + addr );
						*pBuf++ = ( ( szRead & 0xff00 ) >> 8 );
						*pBuf++ = ( szRead & 0x00ff);
						szRead = *( pReg->pData + addr + 1 );
						*pBuf++ = ( ( szRead & 0xff00 ) >> 8 );
						*pBuf++ = ( szRead & 0x00ff);
					}else
						continue;
				}
			}
		}
		API_ProtocolSend( DEV_FOR_GUI, senBuf,  pBuf - senBuf );
	}
	UpdateLastStata();
	g_sys.fncCallBack.pScanEnd();
}

int API_ReadBit( mbyte nCoil, uint16 addr, mbyte* pData, int szByte )
{
	int nRead = szByte;
	LPReg pReg;
	if( GetFormatCoil( nCoil, addr, &pReg, &addr ) == RES_OK && pReg->pState )
	{
		while( nRead )
		{
			if( addr < pReg->sz )
				*pData++ = GETBIT( pReg->pState,addr );
			++addr;
			--nRead;
		}
	}
	return ( szByte - nRead );
}

int API_WriteBit( mbyte nCoil, uint16 addr, mbyte* pData, int szByte )
{
	int nWrite = szByte;
	LPReg pReg;
	if( GetFormatCoil( nCoil, addr, &pReg, &addr ) == RES_OK && pReg->pState )
	{
		while( nWrite )
		{
			if( addr < pReg->sz )
			{
				WriteBitCoil( pReg,addr, *pData++ );
			}
			++addr;
			--nWrite;
		}
	}
	return ( szByte - nWrite );
}

int API_ReadData( mbyte nCoil, uint16 addr, uint16* pData, int szU16 )
{
	LPReg pReg;
	int nRead = szU16;
	uint16* pSource;
	if( GetFormatCoil( nCoil, addr, &pReg, &addr ) == RES_OK && pReg->pData && addr < pReg->sz )
	{
		if( nCoil == api_coilDC && addr >= DCOUNTER_ADDR )
			addr = addr + ( ( addr - DCOUNTER_ADDR ) << 1 );
		pSource = pReg->pData + addr;
		while( nRead && addr++ < pReg->sz )
		{
			*pData++ = *pSource++;
			--nRead;
		}
	}
	return ( szU16 - nRead );
}

int API_WriteData( mbyte nCoil, uint16 addr, uint16* pData, int szU16 )
{
	LPReg pReg;
	uint16* pSource;
	int nWrite = szU16;
	if( GetFormatCoil( nCoil, addr, &pReg, &addr ) == RES_OK && pReg->pData && addr < pReg->sz )
	{
		if( nCoil == api_coilDC && addr >= DCOUNTER_ADDR )
			addr = addr + ( ( addr - DCOUNTER_ADDR ) << 1 );
		pSource = pReg->pData + addr;
		while( nWrite && addr++ < pReg->sz )
		{
			*pSource++ = *pData++;
			--nWrite;
		}
	}
	return ( szU16 - nWrite );
}

int API_ProGramRead( uint16 offset, uint16* pData, int szU16 )
{
	int nRead = szU16;
	uint16* pSource = g_sys.pFlash + offset;
	while( nRead && offset++ < g_sys.szFlash )
	{
		*pData++ = *pSource++;
		--nRead;
	}
	return ( szU16 - nRead );
}

int API_ProGramWrite( uint16 offset, uint16* pData, int szU16 )
{
	int nWrite = szU16;
	uint16* pSource = g_sys.pFlash + offset;
	while( nWrite && offset++ < g_sys.szFlash )
	{
		*pSource++ = *pData++;
		--nWrite;
	}
	return ( szU16 - nWrite );
}

void API_SetInput( uint16 u16Group, uint16 d )
{
	static LPReg pReg = g_sys.R + REG_X;
	uint16 bsz;
	uint16 uBlockBegin, uBlockEnd;
	bsz = pReg->uBlocksz << 4;
	uBlockBegin = u16Group * 16;
	uBlockEnd = uBlockBegin + 16;
	
	*( pReg->pState + u16Group ) = d;
	uBlockBegin /= bsz;
	uBlockEnd /= bsz;
	while( uBlockBegin <= uBlockEnd )
	{
		SETBIT( (uint16*)&pReg->uUpdateFlag, uBlockBegin );
		++uBlockBegin;
	}
}

uint16 API_GetInput( uint16 u16Group )
{
	static LPReg pReg = g_sys.R + REG_X;
	return *( pReg->pState + u16Group );
}

void API_SetOutput( uint16 u16Group, uint16 d )
{
	static LPReg pReg = g_sys.R + REG_Y;
	uint16 bsz;
	uint16 uBlockBegin, uBlockEnd;
	bsz = pReg->uBlocksz << 4;
	uBlockBegin = u16Group * 16;
	uBlockEnd = uBlockBegin + 16;

	*( pReg->pState + u16Group ) = d;
	uBlockBegin /= bsz;
	uBlockEnd /= bsz;
	while( uBlockBegin <= uBlockEnd )
	{
		SETBIT( (uint16*)&pReg->uUpdateFlag, uBlockBegin );
		++uBlockBegin;
	}
}

uint16 API_GetOutput( uint16 u16Group )
{
	static LPReg pReg = g_sys.R + REG_Y;
	return *( pReg->pState + u16Group );
}

int API_IsPLCRuning( void )
{
	return GETBIT( g_sys.R[ REG_M ].pState,SYSADDRM_RUN );
}

void API_UninitSystem( void )
{
	CLRBIT( g_sys.R[ REG_M ].pState, SYSADDRM_SYSINIT );
	CLRBIT( g_sys.R[ REG_M ].pState, SYSADDRM_RUN );
	//nothing can be do
}

void API_StopMachine( int aix )
{
	WriteBitCoil( g_sys.R + REG_Y, aix + MACHINE_Y_OFFSET, 0 );
	//WriteBitCoil( g_sys.R + REG_M, aix + SYSADDRM_MACHINESTOP1, 0 );
}

void API_FeedWDG( void )
{
	g_sys.wdgCount = 0;
}

void API_SetWDG( uint16 ums )
{
	g_sys.wdgSet = ums;
}

void API_TimerUp( void )
{
	mbyte* pTemp;
	CoilData cTimeOut;
	LPStaticLink *ppNod;
	LPStaticLink pNod;
	RegGet regGet;
	//if( !GETBIT( g_sys.R[ REG_M ].pState,SYSADDRM_RUN )  )
	//	return;
	++g_sys.nTimerCount;
	++g_sys.uScanTime;
	if( g_sys.wdgSet > 0 )
	{
		++g_sys.wdgCount;
		if( g_sys.wdgCount >= g_sys.wdgSet )
		{
			SetRunTimeError( eErrWDG,0,0,0 );
			return;
		}
	}
	
	ppNod = &g_sys.pEnT1ms;
	while( *ppNod != NULL )
	{
		(*(*ppNod)->pCounter)++;
		cTimeOut.udata16 = 0;
		pTemp = (*ppNod)->pTimeOut;
		M_GetReg( &pTemp,&regGet );
		M_GetCoilData( eUData16,&regGet,&cTimeOut );
		if( *( (*ppNod)->pCounter ) >= cTimeOut.udata16 )
		{
			pNod = *ppNod;
			if( (*ppNod)->id < 246 )
				*( (*ppNod)->pCounter ) = 0;
			WriteBitCoil( g_sys.R + REG_T,pNod->id, 1 );
			//SETBIT( g_sys.R[REG_T].pState, pNod->id );
			*ppNod = ( *ppNod )->pNext;
			FreeTimerNod( pNod );
			continue;
		}
		ppNod = &( *ppNod )->pNext;
	}
	if( g_sys.nTimerCount % 10 == 0 )
	{
		ppNod = &g_sys.pEnT10ms;
		while( *ppNod != NULL )
		{
			(*(*ppNod)->pCounter)++;
			cTimeOut.udata16 = 0;
			pTemp = (*ppNod)->pTimeOut;
			M_GetReg( &pTemp,&regGet );
			M_GetCoilData( eUData16,&regGet,&cTimeOut );
			if( *( (*ppNod)->pCounter ) >= cTimeOut.udata16 )
			{
				pNod = *ppNod;
				if( (*ppNod)->id < 246 )
					*( (*ppNod)->pCounter ) = 0;
				WriteBitCoil( g_sys.R + REG_T,pNod->id, 1 );
				//SETBIT( g_sys.R[REG_T].pState, pNod->id );
				*ppNod = ( *ppNod )->pNext;
				FreeTimerNod( pNod );
				continue;
			}
			ppNod = &( *ppNod )->pNext;
		}
	}
	if( g_sys.nTimerCount == 100 )
	{
		ppNod = &g_sys.pEnT100ms;
		while( *ppNod != NULL )
		{
			(*(*ppNod)->pCounter)++;
			cTimeOut.udata16 = 0;
			pTemp = (*ppNod)->pTimeOut;
			M_GetReg( &pTemp,&regGet );
			M_GetCoilData( eUData16,&regGet,&cTimeOut );
			if( *( (*ppNod)->pCounter ) >= cTimeOut.udata16 )
			{
				pNod = *ppNod;
				if( (*ppNod)->id < 246 )
					*( (*ppNod)->pCounter ) = 0;
				WriteBitCoil( g_sys.R + REG_T,pNod->id, 1 );
				//SETBIT( g_sys.R[REG_T].pState, pNod->id );
				*ppNod = ( *ppNod )->pNext;
				FreeTimerNod( pNod );
				continue;
			}
			ppNod = &( *ppNod )->pNext;
		}
		g_sys.nTimerCount = 0;
	}
	return;
}

api_res API_LoadProgram( void )
{
	uint16 addr;

	//ResetMem();
	ResetTimerList();
	CleanError();
	//function stack
	for ( addr = 0; addr < REGSZ_P; ++addr )
		g_sys.R[REG_P].pData32[ addr ] = REGP_INVALUED;

	if( M_LoadProgram( &g_sys ) == RES_OK )
	{
		CLRBIT( g_sys.R[ REG_M ].pState, SYSADDRM_SYSINIT );
		CLRBIT( g_sys.R[ REG_M ].pLastState,SYSADDRM_SYSINIT );
		WriteBitCoil( g_sys.R + REG_M, SYSADDRM_SYSINIT, 1 );
		CLRBIT( g_sys.R[ REG_M ].pState,SYSADDRM_RUN );
		CLRBIT( g_sys.R[ REG_M ].pLastState,SYSADDRM_RUN );
		WriteBitCoil( g_sys.R + REG_M, SYSADDRM_RUN, 1 );
		CLRBIT( g_sys.R[ REG_M ].pLastState,SYSADDRM_INITP );
		CLRBIT( g_sys.R[ REG_M ].pState,SYSADDRM_INITP );
		WriteBitCoil( g_sys.R + REG_M, SYSADDRM_INITP, 1 );
		CLRBIT( g_sys.R[ REG_M ].pLastState,SYSADDRM_RELOAD );
		CLRBIT( g_sys.R[ REG_M ].pState,SYSADDRM_RELOAD );
		WriteBitCoil( g_sys.R + REG_M, SYSADDRM_RELOAD, 1 );

		g_sys.bMatherLine = 0;
		g_sys.pPlcStep = g_sys.pPlcMain;
		g_sys.iStl = -1;
		g_sys.nTimerCount = 0;
		return RES_OK;
	}
	return RES_ERR;
}

api_res API_ResetPLC( void )
{
	//必须暂停才能复位
	if( GETBIT( g_sys.R[ REG_M ].pState,SYSADDRM_RUN ) )
		return RES_ERR;

	ZeroMem( &g_sys );
	ResetTimerList();

	//初始化系统信息


	//设置标志位
	CLRBIT( g_sys.R[ REG_M ].pState, SYSADDRM_SYSINIT );
	CLRBIT( g_sys.R[ REG_M ].pLastState,SYSADDRM_SYSINIT );
	WriteBitCoil( g_sys.R + REG_M, SYSADDRM_SYSINIT, 1 );
	CLRBIT( g_sys.R[ REG_M ].pState,SYSADDRM_RUN );
	CLRBIT( g_sys.R[ REG_M ].pLastState,SYSADDRM_RUN );
	WriteBitCoil( g_sys.R + REG_M, SYSADDRM_RUN, 1 );
	CLRBIT( g_sys.R[ REG_M ].pLastState,SYSADDRM_INITP );
	CLRBIT( g_sys.R[ REG_M ].pState,SYSADDRM_INITP );
	WriteBitCoil( g_sys.R + REG_M, SYSADDRM_INITP, 1 );
	CLRBIT( g_sys.R[ REG_M ].pLastState,SYSADDRM_RELOAD );
	CLRBIT( g_sys.R[ REG_M ].pState,SYSADDRM_RELOAD );
	WriteBitCoil( g_sys.R + REG_M, SYSADDRM_RELOAD, 1 );
	return RES_OK;
}

api_res API_InitSystem( LPSysInitInfo pInitInfo )
{
	int i;
	ProtocolConf proConf;
	tiny_mest( &g_sys, 0, sizeof( g_sys ) );
	g_sys.R[ REG_X ].sz = REGSZ_X;
	g_sys.R[ REG_Y ].sz = REGSZ_Y;
	g_sys.R[ REG_M ].sz = REGSZ_M;
	g_sys.R[ REG_S ].sz = REGSZ_S;

	g_sys.R[ REG_T ].sz = REGSZ_T;
	g_sys.R[ REG_C ].sz = REGSZ_C;
	g_sys.R[ REG_D ].sz = REGSZ_D;
	g_sys.R[ REG_P ].sz = REGSZ_P;

	g_sys.R[ REG_L ].sz = REGSZ_L;
	g_sys.R[ REG_V ].sz = REGSZ_V;

	//如果使用数组初始化，则调用以下函数就ok了
	if(  UpdateMem( &g_sys,g_pByteMap, BYTEMAP_SZ,g_pBitMap, BITMAP_SZ ) != RES_OK )
		return RES_ERR;
	//ZeroMem( &g_sys );
	UpdateRegBlocksz();	
	//ResetTimerList();
	InitInsClassFuns();

	EmptyStack( &g_sys.bStack );
	EmptyStack( &g_sys.mStack );
	EmptyStack( &g_sys.pFStack );

	//运行空间 更改将程序加载到希望的地方
	g_sys.pFlash = (uint16*)pInitInfo->pFlash;
	g_sys.szFlash = pInitInfo->szFlash;
	g_sys.pPlcMain = pInitInfo->pRam;
	g_sys.pPlcPrgEnd = pInitInfo->pRam + pInitInfo->szRam;
	g_sys.fncCallBack = pInitInfo->callBack;
	
	//回调函数不允许为空
	if( g_sys.fncCallBack.pScanEnd == NULL
		|| g_sys.fncCallBack.pSetOutPut == NULL
		|| g_sys.fncCallBack.pMachine == NULL
		|| g_sys.fncCallBack.pPLCStop == NULL
		|| g_sys.fncCallBack.pMachine == NULL )
		return RES_ERR;
	for ( i = 0; i < SZ_DEV; ++i )
	{
		if( pInitInfo->callBack.pSend[i] == NULL )
			return RES_ERR;
		proConf.write = pInitInfo->callBack.pSend[i];
		SetProtocolConf( i, &proConf, PROTOCOL_CONF_WRITEFUN );
	}
	API_SetWDG(300);
	API_FeedWDG();
	API_ResetPLC();
	
	return RES_OK;
}

//一进入就不会返回，除非uninitsystem
api_res API_PLCRun( void )
{
	uint16* pMRegStata = ( g_sys.R + REG_M )->pState ;
	uint16* pStep = g_sys.R[REG_D].pData + SYSADDRD_SCANSTEP;
	while( GETBIT( pMRegStata,SYSADDRM_SYSINIT ) )
	{
		if( GETBIT( pMRegStata,SYSADDRM_RUN ) )
		{
			//以下为运行做准备
			if( GETBIT( pMRegStata,SYSADDRM_RELOAD )  )
			{
				if( API_LoadProgram() == RES_ERR )
				{
					SETBIT( pMRegStata, SYSADDRM_ERROR );
					CLRBIT( pMRegStata,SYSADDRM_RUN );
				}
				CLRBIT( pMRegStata,SYSADDRM_RELOAD );
			}
			while( GETBIT( pMRegStata,SYSADDRM_RUN ) ) 
			{
				SetScanPos( g_sys.pPlcPrgEnd );
				if( API_PLCScan() == RES_ERR ) 
				{
					SETBIT( pMRegStata, SYSADDRM_ERROR );
					CLRBIT( pMRegStata,SYSADDRM_RUN );
				}else
				{
					PLCScanEnd();
				}
			}
		}else
		{
			if ( GETBIT( pMRegStata,SYSADDRM_FORCEMODE ) ) //进入强制模式（调试模式）
			{
				if( GETBIT( pMRegStata,SYSADDRM_RELOAD )  )
				{
					if( API_LoadProgram() == RES_ERR )
					{
						SETBIT( pMRegStata, SYSADDRM_ERROR );
						CLRBIT( pMRegStata,SYSADDRM_RUN );
					}
					CLRBIT( pMRegStata,SYSADDRM_RELOAD );
				}else if( GETBIT( pMRegStata,SYSADDRM_FORCERUN ) )
				{
					SetScanPos( NULL );
					if( g_sys.pPlcStep == g_sys.pPlcMain )
					{
						*pStep = 1;
						PLCScanEnd();
					}else
						++(*pStep);
					if( API_PLCScan() == RES_ERR ) 
					{
						SETBIT( pMRegStata, SYSADDRM_ERROR );
						CLRBIT( pMRegStata,SYSADDRM_RUN );
					}
					if( GETBIT( pMRegStata,SYSADDRM_FORCESTOP ) || g_sys.R[REG_D].pData[ SYSADDRD_STEMPSET ] == *pStep )
						CLRBIT( pMRegStata,SYSADDRM_FORCERUN );
				}else{
					g_sys.fncCallBack.pPLCStop();
				}
			}else
			{
				//plc停止运行将执行以下代码
				g_sys.fncCallBack.pPLCStop();
			}
		}
	}
	return RES_OK;
}

api_res API_PLCScan( void )
{
	mbyte iClass;
	LPPLCIns pins;
	RegGet argv[MAX_ARGC];
	if( g_sys.pPlcStep == NULL )
		return RES_ERR;
	do
	{
		if( GetInsLine( &g_sys.pPlcStep, &pins, MAX_ARGC, argv ) == RES_ERR || ( iClass = (mbyte)pins->iclass ) >= g_sys.iClassFuns.sz )
			return SetRunTimeError( eErrScan, 0, g_sys.pPlcStep - g_sys.pPlcMain, 0 );
		else
		{
			switch( g_sys.iClassFuns.funs[iClass]( pins, MAX_ARGC, argv ) )
			{
			case eIFunContinue:
				break;
			case eIFunErr:
				return SetRunTimeError( eErrScan, 0, g_sys.pPlcStep - g_sys.pPlcMain, 0 );
			case eIFunEnd:
				g_sys.pPlcStep = g_sys.pPlcMain;
				return RES_OK;
			}
		}

	}while ( g_sys.pPlcStep < g_sys.pPlcScanPos );
	return RES_OK;
}
