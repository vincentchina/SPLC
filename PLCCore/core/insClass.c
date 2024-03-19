/*
 *	第三个版本，解析运行
 *	将设备相关分离开来
 */
//core 第二版本 使用内存块

#include "PLCCore.h"
#include "MemManager.h"
#include "insClass.h"

extern PLCSystem g_sys;


api_res GetInsLine( mbyte** ppStep, LPPLCIns *pins, mbyte argc, LPRegGet argv )
{
	uint16 uArgc;
	*pins = (LPPLCIns)*ppStep;
	uArgc = (*pins)->iargc - 1;
	if( (*pins)->iargc == 0 || argc < uArgc )
		return RES_ERR;
	M_SkipData16( *ppStep );
	while( uArgc-- )
		M_GetReg( ppStep, argv++ );
	return RES_OK;
}

api_res SkipInsLine( mbyte** ppStep )
{
	mbyte coil;
	//mbyte coilp;
	uint16 uArgc;
	LPPLCIns pins = (LPPLCIns)*ppStep;
	uArgc = pins->iargc - 1;
	M_SkipData16( *ppStep );
	while( uArgc-- )
	{
		coil = **ppStep;
		++( *ppStep );
		if( coil == REG_PV )
		{
			M_SkipData16( *ppStep );
			//coilp = *g_sys.pPlcStep;
			++( *ppStep );
			M_SkipData16( *ppStep );
		}else if( coil == REG_DK )
			M_SkipData32( *ppStep );
		else
			M_SkipData16( *ppStep );
	}
	return RES_OK;
}

static void SkipP( uint16 addrP )
{
	LPPLCIns pins;
	RegGet argv[MAX_ARGC];
	mbyte* jmpP;
	if( g_sys.R[REG_P].pData32[ addrP ] == REGP_INVALUED )
		return;
	jmpP = g_sys.pPlcMain + g_sys.R[REG_P].pData32[ addrP ];
	while( g_sys.pPlcStep != jmpP )
	{
		if( ( ((LPPLCIns)g_sys.pPlcStep)->all & 0x0f0f ) == 0x0601 )//OUT OUTT OUTC
		{
			GetInsLine( &g_sys.pPlcStep, &pins, MAX_ARGC, argv );
			if( pins->all == dinsOUT ){

			}else if( pins->all == dinsOUTT )
			{
				ReMoveTimerNod( (mbyte)argv[1].addr );
				WriteBitCoil( g_sys.R + argv[1].iReg, argv[1].addr,0 );
			}else if( pins->all == dinsOUTC )
			{
				WriteBitCoil( g_sys.R + argv[1].iReg, argv[1].addr,0 );
			}
		}else
			SkipInsLine( &g_sys.pPlcStep );
	}
}
static void SkipSTL( mbyte bClear )
{
	LPPLCIns pins;
	RegGet argv[MAX_ARGC];
	while( ((LPPLCIns)g_sys.pPlcStep)->all != dinsRET && ((LPPLCIns)g_sys.pPlcStep)->all != dinsSTL ){
		if( bClear && 
			( ((LPPLCIns)g_sys.pPlcStep)->all == dinsOUT 
			|| ((LPPLCIns)g_sys.pPlcStep)->all == dinsOUTC
			|| ((LPPLCIns)g_sys.pPlcStep)->all == dinsOUTT
			|| ((LPPLCIns)g_sys.pPlcStep)->all == dinsOUTS ) )//OUT OUTT OUTC
		{
			GetInsLine( &g_sys.pPlcStep, &pins, MAX_ARGC, argv );
			if( pins->all == dinsOUT ){
				WriteBitCoil( g_sys.R + argv[0].iReg, argv[0].addr,0 );
			}else if( pins->all == dinsOUTT )
			{
				ReMoveTimerNod( (mbyte)argv[1].addr );
				WriteBitCoil( g_sys.R + argv[1].iReg, argv[1].addr,0 );
			}else if( pins->all == dinsOUTC )
			{
				WriteBitCoil( g_sys.R + argv[1].iReg, argv[1].addr,0 );
			}
		}else
			SkipInsLine( &g_sys.pPlcStep );
	}
}

/*
static void ClearSTL( mbyte* pStep )
{
	LPPLCIns pins;
	RegGet argv[MAX_ARGC];
	GetInsLine( &pStep, &pins, MAX_ARGC, argv );
	WriteBitCoil( &g_sys.R[ argv->iReg ],argv->addr, 0 );
	while( ((LPPLCIns)pStep)->all != dinsRET && ((LPPLCIns)pStep)->all != dinsSTL )
	{
		if( ( ((LPPLCIns)pStep)->all & 0x0f0f ) == 0x0601 )//OUT OUTT OUTC
		{
			GetInsLine( &pStep, &pins, MAX_ARGC, argv );
			if( pins->all == dinsOUT ){

			}else if( pins->all == dinsOUTT )
			{
				ReMoveTimerNod( (mbyte)argv[1].addr );
				WriteBitCoil( g_sys.R + argv[1].iReg, argv[1].addr,0 );
			}else if( pins->all == dinsOUTC )
			{
				WriteBitCoil( g_sys.R + argv[1].iReg, argv[1].addr,0 );
			}
		}else
			SkipInsLine( &pStep );
	}
}*/

enum eInsFunRes InsClass1( LPPLCIns pins, int argc, LPRegGet argv )
{
	mbyte b;
	LPReg pReg; 

	pReg = g_sys.R + argv[0].iReg;
	b = GETBIT( pReg->pState,argv[0].addr );
	if( pins->all == dinsLDF )
	{
		b = ( !b && GETBIT( pReg->pLastState,argv[0].addr ));
		UpdateBitCoil( pReg, argv[0].addr );
	}else if( pins->all == dinsLDP )
	{
		b = ( b && !GETBIT( pReg->pLastState,argv[0].addr ));
		UpdateBitCoil( pReg, argv[0].addr );
	}else if( pins->all == dinsLDI )
		b = !b;
	BitStackPush( &g_sys.bStack, g_sys.bMatherLine );
	g_sys.bMatherLine = b;
	return eIFunContinue;
}

enum eInsFunRes InsClass2( LPPLCIns pins, int argc, LPRegGet argv )
{
	mbyte b;
	LPReg pReg; 

	pReg = g_sys.R + argv[0].iReg;
	b = GETBIT( pReg->pState,argv[0].addr );
	if( pins->itype == 3 )
	{
		if( pins->all == dinsORI )
			b = !b;
		else if( pins->all == dinsORF )
		{
			b = ( !b && GETBIT( pReg->pLastState,argv[0].addr ));
			UpdateBitCoil( pReg, argv[0].addr );
		}else if( pins->all == dinsORP )
		{
			b = ( b && !GETBIT( pReg->pLastState,argv[0].addr ));
			UpdateBitCoil( pReg, argv[0].addr );
		}
		g_sys.bMatherLine |= b;
	}else
	{
		if( pins->all == dinsANI )
			b = !b;
		else if( pins->all == dinsANDF )
		{
			b = ( !b && GETBIT( pReg->pLastState,argv[0].addr ));
			UpdateBitCoil( pReg, argv[0].addr );
		}else if( pins->all == dinsANDP )
		{
			b = ( b && !GETBIT( pReg->pLastState,argv[0].addr ));
			UpdateBitCoil( pReg, argv[0].addr );
		}
		g_sys.bMatherLine &= b;
	}
	return eIFunContinue;
}
enum eInsFunRes InsClass3( LPPLCIns pins, int argc, LPRegGet argv )
{
	mbyte b;

	b = BitStackRead( &g_sys.bStack );
	StackPop( &g_sys.bStack );
	if( pins->all == dinsANB )
		g_sys.bMatherLine &= b;
	else
		g_sys.bMatherLine |= b;

	return eIFunContinue;
}
enum eInsFunRes InsClass4( LPPLCIns pins, int argc, LPRegGet argv )
{
	if( pins->all == dinsMPS )
	{
		BitStackPush( &g_sys.mStack, g_sys.bMatherLine );
	}else if( pins->all <= dinsMPP )
	{
		g_sys.bMatherLine = BitStackRead( &g_sys.mStack );
		if( pins->all == dinsMPP ) StackPop( &g_sys.mStack );
	}
	return eIFunContinue;
}
enum eInsFunRes InsClass5( LPPLCIns pins, int argc, LPRegGet argv )
{
	mbyte b;
	LPReg pReg; 

	if( pins->all == dinsINV )
	{
		g_sys.bMatherLine = !g_sys.bMatherLine;
	}else
	{
		pReg = g_sys.R + argv[0].iReg;
		b = GETBIT(  g_sys.R[ REG_L ].pState, argv[0].addr );
		if( pins->all == dinsMEP )
			b = ( !b && g_sys.bMatherLine );
		else
			b = ( b && !g_sys.bMatherLine );
		WriteBitCoil( pReg,argv[0].addr, b );
		b ? SETBIT( g_sys.R[ REG_L ].pState, argv[0].addr ) : CLRBIT( g_sys.R[ REG_L ].pState, argv[0].addr );
	}

	return eIFunContinue;
}
enum eInsFunRes InsClass6( LPPLCIns pins, int argc, LPRegGet argv )
{
	mbyte b;
	LPReg pReg; 
	CoilData cData;
	uint16* pCounter1;
	int32* pCounter2;

	StackPop( &g_sys.bStack );
	if( pins->itype != 0xf )
	{
		pReg = g_sys.R + argv[1].iReg;
		if( pins->all == dinsPLS )
		{
			if( g_sys.bMatherLine && !GETBIT( g_sys.R[ argv[0].iReg ].pState, argv[0].addr ) )
				g_sys.fncCallBack.pSetOutPut( argv[1].addr, 1 );
			WriteBitCoil( pReg, argv[1].addr,0 );
		}
		else if( pins->all == dinsPLF )
		{
			if( !g_sys.bMatherLine && GETBIT( g_sys.R[ argv[0].iReg ].pState, argv[0].addr ) )
				g_sys.fncCallBack.pSetOutPut( argv[1].addr, 1 );
			WriteBitCoil( pReg, argv[1].addr,0 );
		}else if ( pins->all == dinsOUTC )
		{
			if( argv[1].iReg != REG_C )
				return eIFunErr;
			if( GETBIT( pReg->pState, argv[1].addr ) )
				return eIFunContinue;
			b = GETBIT( g_sys.R[ REG_L ].pState, argv[0].addr );
			if( argv[1].addr < DCOUNTER_ADDR ) //单向计数
			{
				cData.udata16 = 0;
				pCounter1 = pReg->pData + argv[1].addr;
				M_GetCoilData( eUData16,&argv[2],&cData );
				if( g_sys.bMatherLine && !b )
				{
					( *pCounter1 )++;
					if( *pCounter1 >= cData.udata16 )
						WriteBitCoil( pReg, argv[1].addr,1 );
				}
			}else //双向计数
			{
				cData.udata32 = 0;
				pCounter2 = (int32*)( pReg->pData + DCOUNTER_ADDR ) + ( argv[1].addr - DCOUNTER_ADDR );
				M_GetCoilData( eUData32,&argv[2],&cData );
				if( g_sys.bMatherLine && !b )
				{
					if( GETBIT( g_sys.R[ REG_M ].pState, SYSADDR_OFFSETM + argv[1].addr ) )
					{
						( *pCounter2 )--;
						b = ( *pCounter2 <= cData.sdata32 );
					}else
					{
						( *pCounter2 )++;
						b = ( *pCounter2 >= cData.sdata32 );
					}
					if( b )
						WriteBitCoil( pReg, argv[1].addr,1 );
				}
			}
		}
		g_sys.bMatherLine ? SETBIT( g_sys.R[ REG_L ].pState, argv[0].addr ) : CLRBIT( g_sys.R[ REG_L ].pState, argv[0].addr );
	}else{
		pReg = g_sys.R + argv[0].iReg;
		if( pins->all == dinsSET )
		{
			if( g_sys.bMatherLine )
				WriteBitCoil( pReg, argv[0].addr,1 );
		}else if( pins->all == dinsRST )
		{
			if( g_sys.bMatherLine )
				WriteBitCoil( pReg, argv[0].addr,0 );
		}else if(  pins->all == dinsOUT ) 
			WriteBitCoil( pReg, argv[0].addr,g_sys.bMatherLine );
		//else if(  pins->all == dinsOUTM ) 
		//	WriteBitCoil( pReg, argv[0].addr,g_sys.bMatherLine );
		else if(  pins->all == dinsRSTTC ) 
		{
			if( argv[0].iReg == REG_T && g_sys.bMatherLine )
			{
				ReMoveTimerNod( (mbyte)argv[0].addr );
				WriteBitCoil( pReg, argv[0].addr,0 );
			}else if( argv[0].iReg == REG_C && g_sys.bMatherLine )
			{
				if( argv[0].addr < DCOUNTER_ADDR ) //单向计数
				{
					pCounter1 = pReg->pData + argv[0].addr;
					( *pCounter1 ) = 0;
					WriteBitCoil( pReg, argv[0].addr,0 );
				}else //双向计数
				{
					pCounter2 = ( (int32*)( pReg->pData + DCOUNTER_ADDR ) ) + argv[0].addr - DCOUNTER_ADDR;
					( *pCounter2 ) = 0;
					WriteBitCoil( pReg, argv[0].addr,0 );
				}
			}
		}else if( pins->all == dinsOUTT )
		{
			if( argv[0].iReg != REG_T )
				return eIFunErr;
			if( g_sys.bMatherLine )
				AddToTimerList( (mbyte)argv[0].addr,argv[1].pSource );
			else
			{
				ReMoveTimerNod( (mbyte)argv[0].addr );
				WriteBitCoil( pReg, argv[0].addr,0 );
			}
		}
	}

	return eIFunContinue;
}
enum eInsFunRes InsClass7( LPPLCIns pins, int argc, LPRegGet argv )
{
	//mbyte b;
	//LPReg pReg; 


	return eIFunContinue;
}
enum eInsFunRes InsClass8( LPPLCIns pins, int argc, LPRegGet argv )
{
	//mbyte b;
	//LPReg pReg; 
	if( pins->all == dinsEND )
	{
		//结束本次扫描
		return eIFunEnd;
		//g_sys.fncCallBack.pScanEnd();
	}
	return eIFunErr;
}

enum eInsFunRes InsClass9( LPPLCIns pins, int argc, LPRegGet argv )
{
	//mbyte b;
	//LPReg pReg; 
	StackPop( &g_sys.bStack );
	if( dinsOUTS == pins->all )
	{
		WriteBitCoil( g_sys.R + REG_S,argv[0].addr, g_sys.bMatherLine );
		if( g_sys.iStl >= 0 ) 
			WriteBitCoil( g_sys.R + REG_S,g_sys.iStl, 0 );
	}else if( pins->all == dinsSTL )
	{
		g_sys.bMatherLine = GETBIT( g_sys.R[REG_S].pState, argv[0].addr );
		if( !g_sys.bMatherLine )
		{
			SkipSTL( GETBIT( g_sys.R[REG_S].pLastState, argv[0].addr ) );
			g_sys.iStl = -1;
			//g_sys.pStl = NULL;
		}else
			g_sys.iStl = argv[0].addr;
	}else if( pins->all == dinsRET )
	{
		g_sys.bMatherLine = 0;
		g_sys.iStl = -1;
		//g_sys.pStl = NULL;
	}else if( pins->all == dinsSETS )
	{
		if( g_sys.bMatherLine )
		{
			WriteBitCoil( g_sys.R + REG_S,argv[0].addr, 1 );
			if( g_sys.iStl >= 0 ) 
				WriteBitCoil( g_sys.R + REG_S,g_sys.iStl, 0 );
			//if( g_sys.pStl )//在stl里面set
			//	ClearSTL( g_sys.pStl );
		}
	}else if( pins->all == dinsRSTS )
	{
		if( g_sys.bMatherLine )
		{
			WriteBitCoil( g_sys.R + REG_S,argv[0].addr, 0 );
			//if( g_sys.pStl && *( g_sys.pStl - 1 ) == argv[0].addr )
			//	ClearSTL( g_sys.pStl );
		}
	}

	return eIFunContinue;
}
enum eInsFunRes InsClassA( LPPLCIns pins, int argc, LPRegGet argv )
{
	mbyte b;
	//LPReg pReg; 
	CoilData cData;

	StackPop( &g_sys.bStack );
	if( pins->all == dinsSRET )
	{
		if( !IsStackEmpty( &g_sys.pFStack ) )
		{
			cData.udata32 = DWordStackRead( &g_sys.pFStack );
			g_sys.bMatherLine = BitStackRead( &g_sys.bFStack );
			StackPop( &g_sys.pFStack );
			StackPop( &g_sys.bFStack );
			g_sys.pPlcStep = g_sys.pPlcMain + cData.udata32;
		}else{
			;
		}
	}else if( pins->all == dinsFEND )
	{
		//结束本次扫描
		return eIFunEnd;
		//g_sys.fncCallBack.pScanEnd();
	}else if( pins->all <= dinsCJP )
	{
		if( pins->all == dinsCJP )
		{
			b = GETBIT( g_sys.R[ REG_L ].pState, argv[0].addr );
			if( b != g_sys.bMatherLine )
				g_sys.bMatherLine ? SETBIT( g_sys.R[ REG_L ].pState, argv[0].addr ) : CLRBIT( g_sys.R[ REG_L ].pState, argv[0].addr );
			b = ( g_sys.bMatherLine && !b );
			UpdateBitCoil( g_sys.R + REG_L, argv[0].addr );
		}else
			b = g_sys.bMatherLine;
		if( b )
		{
			//regGet.addr = *( pInsLine + 1 );
			if( pins->all == dinsCALL )
			{
				//M_StepSkip( &pInsLine );
				BitStackPush( &g_sys.bFStack, g_sys.bMatherLine );
				DWordStackPush( &g_sys.pFStack,g_sys.pPlcStep - g_sys.pPlcMain );
				g_sys.pPlcStep = g_sys.pPlcMain + g_sys.R[REG_P].pData32[ argv[ pins->all == dinsCJP ].addr ];
			}else
				SkipP( argv[ pins->all == dinsCJP ].addr );
		}
	}
	return eIFunContinue;
}


enum eInsFunRes InsClassB( LPPLCIns pins, int argc, LPRegGet argv )
{
	//mbyte b;
	LPReg pReg; 
	CoilData cData0;
	CoilData cData1;
	//CoilData cResult;
	CoilData64 llResult;

	mbyte bD32;
	StackPop( &g_sys.bStack );
	if( !g_sys.bMatherLine )
		return eIFunContinue;
	bD32 = ( pins->itype == 0xe );
	cData0.udata32 = 0;
	cData1.udata32 = 0;
	llResult.udata64 = 0;
	if( pins->ins <= 4 )
	{
		M_GetCoilData( bD32 ? eUData32 : eUData16, argv,&cData0 );
		M_GetCoilData( bD32 ? eUData32 : eUData16, argv + 1,&cData1 );
		pReg = g_sys.R + argv[2].iReg;
		switch( pins->ins )
		{
		case 1:
			llResult.udata64 = cData0.udata32 + cData1.udata32;
			break;
		case 2:
			llResult.udata64 = cData0.udata32 - cData1.udata32;
			break;
		case 3:
			if( bD32 )
				llResult.udata64 = cData0.udata32 * cData1.udata32;
			else
				llResult.udata64 = cData0.udata32 * cData1.udata32;
			break;
		case 4:
			if( cData1.udata32 != 0 )
				llResult.udata64 = cData0.udata32 / cData1.udata32;
			else{
				SetRunTimeError( eErrArithmedic,0,0,0);//error
				return eIFunErr;
			}
			break;
		}
		if( bD32 )
		{
			*( (uint32*)pReg->pData + argv[2].addr ) = llResult.d32L.udata32;
			if( pins->ins == 3 )
				*( (uint32*)pReg->pData + argv[2].addr + 2 ) = llResult.d32H.udata32;
			else if( pins->ins == 4 )
				*( (uint32*)pReg->pData + argv[2].addr + 2 ) = cData0.udata32 % cData1.udata32;
		}else
		{
			pReg->pData[ argv[2].addr] = llResult.d32L.u16L;
			if( pins->ins == 4 )
				pReg->pData[ argv[2].addr + 1] = cData0.udata32 % cData1.udata32;
			else if( pins->ins == 3 )
				pReg->pData[ argv[2].addr + 1] = llResult.d32L.u16H;
		}
	}else if( pins->ins <= 6 )
	{
		pReg = g_sys.R + argv[0].iReg;
		if( bD32 )
		{
			if( pins->all == dinsDDEC )
				--( *( (uint32*)pReg->pData + argv[0].addr ) );
			else
				++( *( (uint32*)pReg->pData + argv[0].addr ) );
		}else
		{
			if( pins->all == dinsDEC )
				--pReg->pData[argv[0].addr];
			else
				++pReg->pData[argv[0].addr];
		}
	}else{
		if( pins->ins == 7 )
		{
			M_GetCoilData( bD32 ? eUData32 : eUData16, argv,&cData0 );
			pReg = g_sys.R + argv[1].iReg;
			if( bD32 )
				*( (uint32*)pReg->pData + argv[1].addr ) = cData0.udata32;
			else
				pReg->pData[argv[1].addr] = cData0.udata16;
		}else if( pins->ins == 8 )
		{
			M_GetCoilData( bD32 ? eUData32 : eUData16, argv,&cData0 );
			M_GetCoilData( bD32 ? eUData32 : eUData16, argv + 1,&cData1 );
			pReg = g_sys.R + argv[2].iReg;
			WriteBitCoil( pReg,argv[2].addr, 0 );
			WriteBitCoil( pReg,argv[2].addr + 1, 0 );
			WriteBitCoil( pReg,argv[2].addr + 2, 0 );
			if( cData0.udata32 > cData1.udata32 )
				WriteBitCoil( pReg,argv[2].addr, 1 );
			else if( cData0.udata32 == cData1.udata32 )
				WriteBitCoil( pReg,argv[2].addr + 1, 1 );
			else
				WriteBitCoil( pReg,argv[2].addr + 2, 1 );
		}
	}

	return eIFunContinue;
}
enum eInsFunRes InsClassC( LPPLCIns pins, int argc, LPRegGet argv )
{
	mbyte b;
	//LPReg pReg; 
	CoilData cData0;
	CoilData cData1;

	mbyte bD32 = ( pins->itype > 6 );
	M_GetCoilData( bD32 ? eUData32 : eUData16, argv,&cData0 );
	M_GetCoilData( bD32 ? eUData32 : eUData16, argv + 1,&cData1 );
	switch( bD32 ? pins->ins - 6 : pins->ins )
	{
	case 1:
		b = ( cData0.udata32 == cData1.udata32 );
		break;
	case 2:
		b = ( cData0.udata32 != cData1.udata32 );
		break;
	case 3:
		b = ( cData0.udata32 > cData1.udata32 );
		break;
	case 4:
		b = ( cData0.udata32 >= cData1.udata32 );
		break;
	case 5:
		b = ( cData0.udata32 < cData1.udata32 );
		break;
	case 6:
		b = ( cData0.udata32 <= cData1.udata32 );
		break;
	default:
		return eIFunErr;
	}
	switch( pins->itype )
	{
	case 0:
		BitStackPush( &g_sys.bStack, g_sys.bMatherLine );
		g_sys.bMatherLine = b;
		break;
	case 2:
		g_sys.bMatherLine &= b;
		break;
	case 3:
		g_sys.bMatherLine |= b;
		break;
	default:
		return eIFunErr;
	}

	return eIFunContinue;
}

enum eInsFunRes InsClassD( LPPLCIns pins, int argc, LPRegGet argv )
{
	mbyte id;
	mbyte b;
	mbyte bD32 = ( pins->itype == 0 );
	CoilData cData0;
	CoilData cData1;
	LPRegGet pRegGet = argv + 2;
	StackPop( &g_sys.bStack );
	
	id = pRegGet->addr - MACHINE_Y_OFFSET;
	b = GETBIT( g_sys.R[ REG_M ].pState, id + SYSADDRM_MACHINESTOP1 );
	if( g_sys.bMatherLine && !b )
	{
		cData0.udata32 = 0;
		cData1.udata32 = 0;
		WriteBitCoil( g_sys.R + REG_M, id + SYSADDRM_MACHINESTOP1, 1 );
		WriteBitCoil( g_sys.R + pRegGet->iReg, pRegGet->addr, 1 );
		M_GetCoilData( bD32 ? eUData32 : eUData16, argv,&cData0 );
		M_GetCoilData( bD32 ? eUData32 : eUData16, argv + 1,&cData1 );
		if( g_sys.fncCallBack.pMachine( pRegGet->addr - MACHINE_Y_OFFSET, cData0.sdata32, cData1.sdata32, pins->ins == 0 ) != 0 )
			return eIFunErr;
	}else if( !g_sys.bMatherLine && b )
	{
		WriteBitCoil( g_sys.R + REG_M, id + SYSADDRM_MACHINESTOP1, 0 );
		WriteBitCoil( g_sys.R + pRegGet->iReg, pRegGet->addr, 0 );
		if( g_sys.fncCallBack.pMachine( pRegGet->addr - MACHINE_Y_OFFSET, 0, 0, pins->ins == 0 ) != 0 )
			return eIFunErr;
	}
	//UpdateBitCoil( g_sys.R + REG_M, argv[0].addr );
	if( !GETBIT( g_sys.R[ pRegGet->iReg ].pState, pRegGet->addr ) )
		WriteBitCoil( g_sys.R + REG_M, id + SYSADDRM_MACHINESTOP1, 0 );
	return eIFunContinue;
}

enum eInsFunRes InsClassF( LPPLCIns pins, int argc, LPRegGet argv )
{
	return eIFunContinue;
}

enum eInsFunRes InsClassErr( LPPLCIns pins, int argc, LPRegGet argv )
{
	return eIFunErr;
};
funInsClass g_insClassFuns[]={
	InsClassErr,
	InsClass1,
	InsClass2,
	InsClass3,
	InsClass4,
	InsClass5,
	InsClass6,
	InsClass7,
	InsClass8,
	InsClass9,
	InsClassA,
	InsClassB,
	InsClassC,
	InsClassD,
	InsClassErr,
	InsClassF,
};

api_res InitInsClassFuns()
{
	g_sys.iClassFuns.funs = g_insClassFuns;
	g_sys.iClassFuns.sz = sizeof( g_insClassFuns ) / sizeof(funInsClass);
	return RES_OK;
}
