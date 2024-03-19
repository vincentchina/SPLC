#include "base.h"
#include "plcc.h"
#include "link_list.h"
#include "Memchecker.h"


#define STR_CHEAD1 "/* \n\
 * this file is output by plcc.exe\n\
 * plc program hex code for debug\n\
 */\n\n\
#include \"apis.h\"\n\n"

#define STR_CHEAD2 "\n\nuint16 plcprg[]={"

#define STR_CTAIL "\n};\n"


static int g_bHex = 0;
static int g_nWrite = 0;


void FormatWriteIns( uint16* data,int sz,FILE* fout )
{
	char strHex[9];
	if( g_bHex )
	{
		while( sz-- )
		{
			sprintf( strHex, "0x%04x, ", *data );
			fwrite( strHex,1,8,fout );
			++data;
			++g_nWrite;
		}
	}else
		fwrite( data,sizeof(uint16),sz,fout );
}

void FormatWriteArgs( uint16* data,int sz,FILE* fout )
{
	char strHex[9];
	if( g_bHex )
	{
		while( sz-- )
		{
			sprintf( strHex, "0x%04x, ", *data );
			fwrite( strHex,1,8,fout );
			++data;
			++g_nWrite;
		}
	}else
		fwrite( data,sizeof(uint16),sz,fout );
}

int CompileCoil( LPInsItem pins, char* pList[], int szlist, uint16* pArg, int sz )
{
	PLCCoil coil;
	uint32 addr;
	char* pstr;
	int nArg = 0;
	int base;
	int dBegine;
	char bVfind;

	while( szlist )
	{
		switch( **pList )
		{
		case 'X': coil.all = dcoilX;break;
		case 'Y': coil.all = dcoilY;break;
		case 'M': coil.all = dcoilM;break;
		case 'S': coil.all = dcoilS;break;
		case 'T': coil.all = dcoilT;break;
		case 'C': coil.all = dcoilC;break;
		case 'D':{
			if( *( (*pList) + 1 ) == 'K' )
				coil.all = dcoilK;
			else
				coil.all = dcoilD;
			break;
		}
		case 'K': coil.all = dcoilK;break;
		case 'P': coil.all = dcoilP;break;
		case 'V': coil.all = dcoilV;break;
		case 'G': coil.all = dcoilG;break;
		default: return -1;
		}
		if( pins->bDK && coil.all == dcoilK )
			coil.all = dcoilDK;
		pstr = *pList + 1;
		while( *pstr != '\0' && *pstr != 'V' ) ++pstr;
		if( *pstr == 'V' )
		{
			*pstr = 0;
			if( !Str2Num( pstr + 1,&addr,10 ) )
				return 0;
			pArg[nArg++] = dcoilPV;
			pArg[nArg++] = addr & 0x0000ffff;
		}

		dBegine = nArg;
		pArg[nArg++] = coil.all;
		addr = 0;
		pstr = *pList + 1;
		if( coil.all == dcoilX || coil.all == dcoilY )
			base = 8;
		else
			base = 10;

		if( !Str2Num( pstr,&addr,base ) )
			return 0;
		if( coil.all == dcoilDK )	//DWORD
		{
			pArg[nArg++] = addr & 0x0000ffff;
			pArg[nArg++] = ( addr & 0xffff0000 ) >> 16;
		}else
		{
			pArg[nArg++] = addr;
		}
		++pList;
		--szlist;
	}
	return nArg;
}
int TestCoil( uint16 format, char* pCoil )
{
	char* strPart;
	switch( *pCoil )
	{
	case 'X': if( format & ARG_COIL_X ) return 0;break;
	case 'Y': if( format & ARG_COIL_Y ) return 0;break;
	case 'M': if( format & ARG_COIL_M ) return 0;break;
	case 'S': if( format & ARG_COIL_S ) return 0;break;
	case 'T': if( format & ARG_COIL_T ) return 0;break;
	case 'C': if( format & ARG_COIL_C ) return 0;break;
	case 'D': if( format & ARG_COIL_D ) return 0;break;
	case 'K': if( format & ARG_COIL_K ) return 0;break;
	case 'P': if( format & ARG_COIL_P ) return 0;break;
	case 'G': if( format & ARG_COIL_G ) return 0;break;
	case 'V': 
		{
			if( format & ARG_COIL_V )
			{
				strPart = pCoil + 1;
				while( *strPart >= '0' && *strPart <= '9' ) ++strPart;
				if( *strPart != '\0' )
					return TestCoil( format, strPart);
				return 0;
			}
		}
		break;
	default: return -1;
	}
	return -1;
}
static int InsLineCompile( char* pList[], int szlist, FILE* fout )
{
	//高4位数据长度，目前只有1、2、3字节
#define ARG_MAX_LEN	20
	LPInsItem pins;
	uint16 uInsArgc;
	uint16 i;
	uint16 pArgList[ARG_MAX_LEN] = {0};
	char* pstr;
	char** pArgv;

	if( szlist < 1 )
		return 0;
	pstr = pList[0];

	if( *pstr == ';' )//note
		return 1;

	if( *pstr == 'P' )
	{
		++pstr;
		while( *pstr != '\0' )
		{
			if( *pstr < '0' || *pstr > '9') break;
			++pstr;
		}
		if( *pstr == '\0' )
			pstr = "P";
		else
			pstr = pList[0];
	}
	if( pList[ szlist - 1][0] == ';' )
		--szlist;
	pins = GetInsItemByStr( pstr );
	while( pins != NULL )
	{
		//特殊指令处理
		if( pins->bin.all != dinsP )
		{
			uInsArgc = szlist - 1;
			pArgv = pList + 1;
		}else{
			uInsArgc = szlist;
			pArgv = pList;
		}
		if( pins->argc == uInsArgc )
		{
			i = uInsArgc;
			while( i )
			{
				if( TestCoil( pins->argv[i - 1], pArgv[i - 1] ) == -1 )
					break;
				--i;
			}
			if( i == 0 )
				break;
		}
		pins = GetNextInsItemByStr( pins );
	}
	if( pins == NULL )
		return -1;

	uInsArgc = pins->argc;
	FormatWriteIns( &pins->bin.all, 1,fout );
	if( uInsArgc )
	{
		if ( ( uInsArgc = CompileCoil( pins, pArgv, uInsArgc, pArgList, ARG_MAX_LEN ) ) <= 0 )
			return -1;
		else
			FormatWriteArgs( pArgList, uInsArgc, fout );		
	}
	if( g_bHex ) fwrite( "\n",1,1,fout );
	return 1;
}

void Str2BinSetHex( int bHex )
{
	g_bHex = bHex;
}
int Str2Bin( LPPLCCData pData )
{	
	unsigned char* pPre;
	unsigned char* pstr;
	int res = -1;
	int nLine = 0;
	int szlist;
	int n;
	unsigned char* pstrEnd;
	char* pInsLine[10];		//最多10个

	g_bHex = pData->bToHex;
	pstr = pData->pSource;
	pstrEnd = pstr + pData->szSource;

	initInsHash();

	fseek( pData->fout,0,SEEK_SET);
	//如果编译成c文件的hex数组 添加头
	if( g_bHex )
	{
		fwrite( STR_CHEAD1,sizeof(char),sizeof(STR_CHEAD1) - 1,pData->fout );
		fwrite( "/*\n",sizeof(char),2,pData->fout );
		fwrite( pData->pSource,sizeof(char),pData->szSource,pData->fout );
		fwrite( "*/\n",sizeof(char),2,pData->fout );
		fwrite( STR_CHEAD2,sizeof(char),sizeof(STR_CHEAD2) - 1,pData->fout );
		fwrite( "\n",1,1,pData->fout );
	}

	while( pstr < pstrEnd )
	{
		n = 0;
		szlist = GetInisLine( (char**)&pstr, pInsLine, 10,&n );
		nLine += n;
		//插入调试信息，如果需要的话
		if( pData->bDebug )
		{
			char* pDebugInsLine[2];
			char strCoil[7];
			sprintf( strCoil, "G%d", nLine );
			pDebugInsLine[0] = "DEBUG";
			pDebugInsLine[1] = strCoil;
			if( InsLineCompile( pDebugInsLine, 2, pData->fout ) <= 0 )
			{
				printf( "插入调试信息错误！\n行数：%d\n", nLine );
				goto leave;
			}
		}
		if( szlist && InsLineCompile( pInsLine, szlist, pData->fout ) <= 0 )
		{
			n = 10;
			pstrEnd = (unsigned char*)*pInsLine;
			while( n-- && *pstrEnd++ != 0 );
			if( n < 0 )
			{
				pstrEnd -= 3;
				*pstrEnd++ = '.';
				*pstrEnd++ = '.';
				*pstrEnd++ = '.';
				*pstrEnd++ = 0;
			}
			printf( "错误的指令格式:\"%s\"\n错误行数：%d\n",*pInsLine, nLine );
			goto leave;
		}
	}
	//如果编译成c文件的hex数组 添加尾
	if( g_bHex )
	{ 
		fwrite( STR_CTAIL,sizeof(char),sizeof(STR_CTAIL) - 1,pData->fout );
		fprintf( pData->fout,"uint16 szprg = %d;\n", g_nWrite );
		fprintf( pData->fout,"uint16 plcprgRam[%d];\n", g_nWrite * 2 );
	}
	res = 0;
leave:
	return res;
}

