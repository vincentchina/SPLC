#include "plcInsSys/plcInsSys.h"
#include "common/misc.h"
#include <string.h>

static int g_bInit = 0;
static const unsigned long g_magicKey = 3559;
#define HASH_LIST_LEN	200
static LPInsItem g_hashList[HASH_LIST_LEN];

static InsItem g_insItems[] = {

	{{dinsDEBUG},	sinsNOP-1,	"DEBUG",	1,0, ARG_COIL_G },

	{{dinsLD},		sinsLD,		"LD",		1,0, ARG_COIL_BIT},	
	{{dinsLDI},		sinsLDI,	"LDI",		1,0, ARG_COIL_BIT},
	{{dinsLDP},		sinsLDP,	"LDP",		1,0, ARG_COIL_BIT},
	{{dinsLDF},		sinsLDF,	"LDF",		1,0, ARG_COIL_BIT},
	{{dinsAND},		sinsAND,	"AND",		1,0, ARG_COIL_BIT},
	{{dinsANI},		sinsANI,	"ANI",		1,0, ARG_COIL_BIT},
	{{dinsANDP},	sinsANDP,	"ANDP",		1,0, ARG_COIL_BIT},
	{{dinsANDF},	sinsANDF,	"ANDF",		1,0, ARG_COIL_BIT},
	{{dinsOR},		sinsOR,		"OR",		1,0, ARG_COIL_BIT},
	{{dinsORI},		sinsORI,	"ORI",		1,0, ARG_COIL_BIT},
	{{dinsORP},		sinsORP,	"ORP",		1,0, ARG_COIL_BIT},
	{{dinsORF},		sinsORF,	"ORF",		1,0, ARG_COIL_BIT},
	{{dinsANB},		sinsANB,	"ANB",		0,0, 0},
	{{dinsORB},		sinsORB,	"ORB",		0,0, 0},
	{{dinsMPS},		sinsMPS,	"MPS",		0,0, 0},
	{{dinsMRD},		sinsMRD,	"MRD",		0,0, 0},
	{{dinsMPP},		sinsMPP,	"MPP",		0,0, 0},
	{{dinsINV},		sinsINV,	"INV",		0,0, 0},
	{{dinsMEP},		sinsMEP,	"MEP",		0,0, 0},
	{{dinsMEF},		sinsMEF,	"MEF",		0,0, 0},

	//下面的问题可能会多点，有很多重复，需要小心
	{{dinsOUT},		sinsOUT,	"OUT",		1,0, ARG_COIL_X | ARG_COIL_Y | ARG_COIL_M },
	{{dinsOUTT},	sinsOUTT,	"OUT",		2,0, ARG_COIL_T,ARG_COIL_DATA},
	{{dinsOUTC},	sinsOUTC,	"OUT",		2,0, ARG_COIL_C,ARG_COIL_DATA},
	{{dinsOUT},		sinsOUTMS,	"OUT",		1,0, ARG_COIL_M },
	{{dinsOUTS},	sinsOUTMS,	"OUT",		1,0, ARG_COIL_S },
	{{dinsSET},		sinsSET,	"SET",		1,0, ARG_COIL_X | ARG_COIL_Y | ARG_COIL_M },
	{{dinsSETS},	sinsSETMS,	"SET",		1,0, ARG_COIL_S },
	{{dinsSET},		sinsSETMS,	"SET",		1,0, ARG_COIL_M },
	{{dinsRST},		sinsRST,	"RST",		1,0, ARG_COIL_X | ARG_COIL_Y | ARG_COIL_M },
	{{dinsRSTTC},	sinsRSTMSTC,"RST",		1,0, ARG_COIL_T | ARG_COIL_C },
	{{dinsRSTS},	sinsRSTMSTC,"RST",		1,0, ARG_COIL_S },
	{{dinsRST},		sinsRSTMSTC,"RST",		1,0, ARG_COIL_M },
	{{dinsPLS},		sinsPLS,	"PLS",		1,0, ARG_COIL_X | ARG_COIL_Y | ARG_COIL_M | ARG_COIL_S },
	{{dinsPLF},		sinsPLF,	"PLF",		1,0, ARG_COIL_X | ARG_COIL_Y | ARG_COIL_M | ARG_COIL_S },

	{{dinsSTL},		sinsSTL,	"STL",		1,0, ARG_COIL_S},
	{{dinsRET},		sinsRET,	"RET",		0,0, 0},
	//上面的问题可能会多点，有很多重复，需要小心

	{{dinsMC},		sinsMC,		"MC",		1,0, ARG_COIL_N },
	{{dinsMCR},		sinsMCR,	"MCR",		1,0, ARG_COIL_N },

	{{dinsNOP},		sinsNOP,	"NOP",		0,0, 0},
	{{dinsEND},		sinsEND,	"END",		0,0, 0},
	
	{{dinsADD},		sinsADD,	"ADD",		3,0, ARG_COIL_DATA,ARG_COIL_DATA,ARG_COIL_D | ARG_COIL_V },
	{{dinsSUB},		sinsSUB,	"SUB",		3,0, ARG_COIL_DATA,ARG_COIL_DATA,ARG_COIL_D | ARG_COIL_V },
	{{dinsMUL},		sinsMUL,	"MUL",		3,0, ARG_COIL_DATA,ARG_COIL_DATA,ARG_COIL_D | ARG_COIL_V },
	{{dinsDIV},		sinsDIV,	"DIV",		3,0, ARG_COIL_DATA,ARG_COIL_DATA,ARG_COIL_D | ARG_COIL_V },
	{{dinsINC},		sinsINC,	"INC",		1,0, ARG_COIL_D | ARG_COIL_V },
	{{dinsDEC},		sinsDEC,	"DEC",		1,0, ARG_COIL_D | ARG_COIL_V },

	{{dinsMOV},		sinsMOV,	"MOV",		2,0, ARG_COIL_DATA,ARG_COIL_D | ARG_COIL_V },
	{{dinsCMP},		sinsCMP,	"CMP",		3,0, ARG_COIL_DATA,ARG_COIL_DATA,ARG_COIL_M},

	{{dinsCJ},		sinsCJ,		"CJ",		1,0, ARG_COIL_P},
	{{dinsCJP},		sinsCJP,	"CJP",		1,0, ARG_COIL_P},

	{{dinsSRET},	sinsSRET,	"SRET",		0,0, 0},
	{{dinsFEND},	sinsFEND,	"FEND",		0,0, 0},
	{{dinsCALL},	sinsCALL,	"CALL",		1,0, ARG_COIL_P},
	{{dinsP},		sinsP,		"P",		1,0, ARG_COIL_P},

	{{dinsDADD},	sinsDADD,	"DADD",		3,1, ARG_COIL_DATA,ARG_COIL_DATA,ARG_COIL_D | ARG_COIL_V},
	{{dinsDSUB},	sinsDSUB,	"DSUB",		3,1, ARG_COIL_DATA,ARG_COIL_DATA,ARG_COIL_D | ARG_COIL_V},
	{{dinsDMUL},	sinsDMUL,	"DMUL",		3,1, ARG_COIL_DATA,ARG_COIL_DATA,ARG_COIL_D | ARG_COIL_V},
	{{dinsDDIV},	sinsDDIV,	"DDIV",		3,1, ARG_COIL_DATA,ARG_COIL_DATA,ARG_COIL_D | ARG_COIL_V},
	{{dinsDINC},	sinsDINC,	"DINC",		1,1, ARG_COIL_D | ARG_COIL_V},
	{{dinsDDEC},	sinsDDEC,	"DDEC",		1,1, ARG_COIL_D | ARG_COIL_V},

	{{dinsDMOV},	sinsDMOV,	"DMOV",		2,1, ARG_COIL_DATA,ARG_COIL_D | ARG_COIL_V},
	{{dinsDCMP},	sinsDCMP,	"DCMP",		3,1, ARG_COIL_DATA,ARG_COIL_DATA,ARG_COIL_M},

	{{dinsLD_E},	sinsLD_E,	"LD=",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsAND_E},	sinsAND_E,	"AND=",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsOR_E},	sinsOR_E,	"OR=",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsLD_NE},	sinsLD_NE,	"LD<>",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsAND_NE},	sinsAND_NE,	"AND<>",	2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsOR_NE},	sinsOR_NE,	"OR<>",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsLD_B},	sinsLD_B,	"LD>",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsAND_B},	sinsAND_B,	"AND>",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsOR_B},	sinsOR_B,	"OR>",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsLD_EB},	sinsLD_EB,	"LD>=",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsAND_EB},	sinsAND_EB,	"AND>=",	2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsOR_EB},	sinsOR_EB,	"OR>=",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsLD_S},	sinsLD_S,	"LD<",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsAND_S},	sinsAND_S,	"AND<",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsOR_S},	sinsOR_S,	"OR<",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsLD_ES},	sinsLD_ES,	"LD<=",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsAND_ES},	sinsAND_ES,	"AND<=",	2,0, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsOR_ES},	sinsOR_ES,	"OR<=",		2,0, ARG_COIL_DATA,ARG_COIL_DATA},


	{{dinsLDD_E},	sinsLDD_E,	"LDD=",		2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsANDD_E},	sinsANDD_E,	"ANDD=",	2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsORD_E},	sinsORD_E,	"ORD=",		2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsLDD_NE},	sinsLDD_NE,	"LDD<>",	2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsANDD_NE},	sinsANDD_NE,"ANDD<>",	2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsORD_NE},	sinsORD_NE,	"ORD<>",	2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsLDD_B},	sinsLDD_B,	"LDD>",		2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsANDD_B},	sinsANDD_B,	"ANDD>",	2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsORD_B},	sinsORD_B,	"ORD>",		2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsLDD_EB},	sinsLDD_EB,	"LDD>=",	2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsANDD_EB},	sinsANDD_EB,"ANDD>=",	2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsORD_EB},	sinsORD_EB,	"ORD>=",	2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsLDD_S},	sinsLDD_S,	"LDD<",		2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsANDD_S},	sinsANDD_S,	"ANDD<",	2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsORD_S},	sinsORD_S,	"ORD<",		2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsLDD_ES},	sinsLDD_ES,	"LDD<=",	2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsANDD_ES},	sinsANDD_ES,"ANDD<=",	2,1, ARG_COIL_DATA,ARG_COIL_DATA},
	{{dinsORD_ES},	sinsORD_ES,	"ORD<=",	2,1, ARG_COIL_DATA,ARG_COIL_DATA},

	{{dinsDDRVA},	sinsDDRVA,	"DDRVA",	4,1, ARG_COIL_DATA, ARG_COIL_DATA, ARG_COIL_Y, ARG_COIL_Y },
	{{dinsDDRVI},	sinsDDRVI,	"DDRVI",	4,1, ARG_COIL_DATA, ARG_COIL_DATA, ARG_COIL_Y, ARG_COIL_Y },
	{{dinsDRVA},	sinsDRVA,	"DRVA",		4,0, ARG_COIL_DATA, ARG_COIL_DATA, ARG_COIL_Y, ARG_COIL_Y },
	{{dinsDRVI},	sinsDRVI,	"DRVI",		4,0, ARG_COIL_DATA, ARG_COIL_DATA, ARG_COIL_Y, ARG_COIL_Y },
};

static LPInsItem g_lstBin[sizeof(g_insItems) / sizeof(InsItem)];
static LPInsItem g_lstWpg[sizeof(g_insItems) / sizeof(InsItem)];

static CoilItem g_coilItems[] = {
	{{dcoilX},	scoilX,		"X"},
	{{dcoilY},	scoilY,		"Y"},
	{{dcoilM},	scoilM,		"M"},
	{{dcoilS},	scoilS,		"S"},
	{{dcoilT},	scoilT,		"T"},
	{{dcoilC},	scoilC,		"C"},
	{{dcoilD},	scoilD,		"D"},
	{{dcoilP},	scoilP,		"P"},
	{{dcoilV},	scoilV,		"V"},
	{{dcoilN},	scoilN,		"N"},

	{{dcoilK},	scoilK,		"K"},
	{{dcoilDK},	scoilDK,	"K"},
	//{{dcoilDT},	scoilX,		"X"},
	//{{dcoilDC},	scoilX,		"X"},
	{{dcoilPV},	scoil_V,	"V"},
	//{{dcoilDDT},scoilX,		"X"},
};

LPInsItem GetInsItemByStr( const char* str )
{
	unsigned long i;
	LPInsItem pitem;
	LPInsItem pfind;
	i =  Hash2Index( HashStr( str ), HASH_LIST_LEN );
	pitem = g_hashList[ i ];
	pfind = pitem;
	do{
		if( pfind == NULL )
			return NULL;
		if( strcmp(str,pfind->str) == 0 )
			return pfind;
		if( ++i > HASH_LIST_LEN )
			i = 0;
		pfind = *( g_hashList + i );
	}while( pfind != pitem );
	return 0;
}


LPInsItem GetNextInsItemByStr( LPInsItem pItem )
{
	unsigned long i;
	LPInsItem pitem;
	LPInsItem pfind;
	i =  Hash2Index( pItem->hash, HASH_LIST_LEN );
	pitem = g_hashList[ i ];
	pfind = pitem;
	do{
		if( pfind == NULL )
			return NULL;
		if( pItem == pfind )
		{
			if( ++i > HASH_LIST_LEN )
				i = 0;
			pfind = g_hashList[ i ];
			while( pfind && pfind != pitem )
			{
				if( pItem->hash == pfind->hash && strcmp( pItem->str,pfind->str) == 0 )
					return pfind;
				if( ++i > HASH_LIST_LEN )
					i = 0;
				pfind = g_hashList[ i ];
			}
			return 0;
		}
		if( ++i > HASH_LIST_LEN )
			i = 0;
		pfind = *( g_hashList + i );
	}while( pfind != pitem );
	return 0;
}

LPInsItem FindInsItemByBin( int nBegin, int nEnd, PLCIns bin )
{
	int nMid = nBegin + ( nEnd - nBegin ) / 2;
	if( nMid == nBegin )
	{
		if( g_lstBin[nMid]->bin.all == bin.all )
			return g_lstWpg[nMid];
		else
			return 0;
	}
	if( g_lstBin[nMid]->bin.all == bin.all )
		return g_lstBin[nMid];
	else if( g_lstBin[nBegin]->bin.all == bin.all )
		return g_lstBin[nBegin];
	else if( g_lstBin[nEnd - 1]->bin.all == bin.all )
		return g_lstBin[nEnd - 1];
	else if( g_lstBin[nMid]->bin.all < bin.all )
		return FindInsItemByBin( nMid, nEnd, bin );
	else if( g_lstBin[nMid]->bin.all > bin.all )
		return FindInsItemByBin( nBegin, nMid, bin );
	else
		return 0;
}

LPInsItem GetInsItemByBin( PLCIns bin )
{
	return FindInsItemByBin( 0, sizeof(g_insItems) / sizeof(InsItem), bin );
}

LPInsItem FindInsItemByWpg( int nBegin, int nEnd, unsigned long uwpg )
{
	int nMid = nBegin + ( nEnd - nBegin ) / 2;
	if( nMid == nBegin )
	{
		if( g_lstWpg[nMid]->uwpg == uwpg )
			return g_lstWpg[nMid];
		else
			return 0;
	}
	if( g_lstWpg[nMid]->uwpg == uwpg )
		return g_lstWpg[nMid];
	else if( g_lstWpg[nBegin]->uwpg == uwpg )
		return g_lstWpg[nBegin];
	else if( g_lstWpg[nEnd - 1]->uwpg == uwpg )
		return g_lstWpg[nEnd - 1];
	else if( g_lstWpg[nMid]->uwpg < uwpg )
		return FindInsItemByWpg( nMid, nEnd, uwpg );
	else if( g_lstWpg[nMid]->uwpg > uwpg )
		return FindInsItemByWpg( nBegin, nMid, uwpg );
	else
		return 0;
}

LPInsItem GetInsItemByWpg( unsigned long uwpg )
{
	return FindInsItemByWpg( 0, sizeof(g_insItems) / sizeof(InsItem), uwpg );
}


LPCoilItem GetCoilItemByStr( const char* str )
{
	for ( int i = 0; i < sizeof( g_coilItems ) / sizeof(CoilItem); ++i )
	{
		if( strcmp( g_coilItems[i].str, str ) == 0)
			return g_coilItems + i;
	}
	return 0;
}

LPCoilItem GetNextCoilItemByStr( LPCoilItem pItem )
{
	for ( int i = 0; i < sizeof( g_coilItems ) / sizeof(CoilItem); ++i )
	{
		if( g_coilItems + i == pItem )
		{
			if( strcmp( g_coilItems[i].str, pItem->str ) == 0 )
				return g_coilItems + i;
			else
				return 0;
		}
	}
	return 0;
}

LPCoilItem GetCoilItemByBin( PLCCoil bin )
{
	for ( int i = 0; i < sizeof( g_coilItems ) / sizeof(CoilItem); ++i )
	{
		if( g_coilItems[i].bin.all == bin.all )
			return g_coilItems + i;
	}
	return 0;
}
LPCoilItem GetCoilItemByWpg( unsigned short uwpg )
{
	for ( int i = 0; i < sizeof( g_coilItems ) / sizeof(CoilItem); ++i )
	{
		if( g_coilItems[i].uwpg == uwpg )
			return g_coilItems + i;
	}
	return 0;
}

int SortInsItems()
{
	if( !g_bInit )
	{
		LPInsItem tmp;
		int nBinMin;
		int nWpgMin;
		for( int i = 0; i < sizeof(g_insItems) / sizeof(InsItem); i++ )
		{
			g_lstBin[i] = g_insItems + i;
			g_lstWpg[i] = g_insItems + i;
		}
		for( int i = 0; i < sizeof(g_insItems) / sizeof(InsItem); i++ )
		{
			nBinMin = i;
			nWpgMin = i;
			for( int j = i + 1; j < sizeof(g_insItems) / sizeof(InsItem); j++ )
			{
				if( g_lstBin[j]->bin.all < g_lstBin[nBinMin]->bin.all )
					nBinMin = j;
				if( g_lstWpg[j]->uwpg < g_lstWpg[nWpgMin]->uwpg )
					nWpgMin = j;
			}
			if( nBinMin != i )
			{
				tmp = g_lstBin[nBinMin];
				g_lstBin[nBinMin] = g_lstBin[i];
				g_lstBin[i] = tmp;
			}
			if( nWpgMin != i )
			{
				tmp = g_lstWpg[nWpgMin];
				g_lstWpg[nWpgMin] = g_lstWpg[i];
				g_lstWpg[i] = tmp;
			}
		}
		g_bInit = 1;
	}
	return g_bInit;
}

int initInsHash()
{
	int i;
	unsigned long index;
	LPInsItem* pitem;
	LPInsItem* pkeyitem;
	if( !g_bInit && SortInsItems() )
	{
		memset( g_hashList,0, sizeof(g_hashList));
		for( i = 0; i < sizeof(g_insItems) / sizeof(InsItem); i++ )
		{
			g_insItems[i].hash = HashStr(g_insItems[i].str);
			index = Hash2Index( g_insItems[i].hash, HASH_LIST_LEN );
			pkeyitem = pitem = g_hashList + index;
			while( *pitem != NULL )
			{
				if( ++index > HASH_LIST_LEN )
					index = 0;
				pitem = g_hashList + index;
				if( pitem == pkeyitem )
					return 0;
			}
			*pitem = &g_insItems[i];
		}
		g_bInit = 1;
	}
	return g_bInit;
}
