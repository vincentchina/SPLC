#include "base.h"
#include "plcc.h"
#include "link_list.h"
#include "Memchecker.h"

extern int GetInisLine(char** pstr, char* pList[], int szlist, int* plineGet );

#define STR_CHEAD "#include \"stdio.h\"\n\
#if 1//_WIN32\n\
typedef unsigned short uint16;\n\
typedef unsigned int uint32;\n\
typedef short api_res;\n\
#else\n\
//DSP without 8 bit\n\
typedef unsigned int uint16;\n\
typedef unsigned long uint32;\n\
typedef int api_res;\n\
#endif\n\
\n\
typedef union tagBitMap{\n\
	uint16 all;\n\
	struct{\n\
		uint16 B0:1;\n\
		uint16 B1:1;\n\
		uint16 B2:1;\n\
		uint16 B3:1;\n\
		uint16 B4:1;\n\
		uint16 B5:1;\n\
		uint16 B6:1;\n\
		uint16 B7:1;\n\
		uint16 B8:1;\n\
		uint16 B9:1;\n\
		uint16 Ba:1;\n\
		uint16 Bb:1;\n\
		uint16 Bc:1;\n\
		uint16 Bd:1;\n\
		uint16 Be:1;\n\
		uint16 Bf:1;\n\
	};\n\
}BitMap,*LPBitMap;\n\
\n\
typedef union tagTimer{\n\
	uint32 all;\n\
	struct{\n\
		uint16 EN:1;	//使能\n\
		uint16 C:15;	//timer id\n\
\n\
		uint16 R:1;		//是否寄存器寻址\n\
		uint16 N:15;	//地址或立即数\n\
	};\n\
}Timer,*LPTimer;\n\
\n\
typedef struct tagReg{\n\
	LPBitMap X;\n\
	LPBitMap Y;\n\
	LPBitMap S;\n\
	LPBitMap M;\n\
	LPBitMap N;\n\
	LPBitMap T;\n\
	LPBitMap L;\n\
	LPBitMap Z;\n\
	LPBitMap C;\n\
\n\
	uint16* U;\n\
	uint16* D;\n\
\n\
	uint32* W;\n\
	LPTimer V;\n\
}PLCRunData,*LPPLCRunData;\n\
\n\
//extern \"C\" __declspec(dllexport) \n\
int plcLogicRun( LPPLCRunData p )\n\
{\n\
\n\
"

#define STR_CTAIL "\nreturn 0;\n}\n"

enum compileState{
	COMPILE_NEW,
	COMPILE_CONTINUE,
};

static slListHead g_lstBlock;	//保存程序块
static slListHead g_lstOP;		//保存操作符
static slListHead g_lstNum;	//保存操作数
static slListHead g_lstStr;	//保存临时翻译后的指令

typedef struct tagMCBlock{
	char n[6];
	char m[6];
}MCBlock,*LPMCBlock;
static MCBlock g_McBlock[8] = {0};	//N0~N7
static LPMCBlock g_Mc = NULL;		//指向当前nc块
static char* g_motherLine = NULL;

#define NTMP_
static char g_btmpset = 0;	//g_ntmp 重置标志位 insMPP后再insOUT的第一个LD重置
static int g_ntmp = 0;
static int g_nstatic = 0;

char* FormatCoil( char* strCiol )
{
#if 0
	char* strOut;
	strOut = (char*)MEMNEW( strlen(strCiol) + 1 );
	strcpy( strOut, strCiol );
	return strOut;
#else
	int n,m;
	char* strOut;
	char* pstr;
	char c = *strCiol;
	int nCiol = atoi( strCiol + 1);
	strOut = (char*)MEMNEW( strlen(strCiol) + 20 );
	sprintf( strOut, "p->%C[",c);
	pstr = strOut + strlen(strOut);
	if( c == 'X'			//bit
		|| c == 'Y'
		|| c == 'S' 
		|| c == 'M' 
		|| c == 'N' 
		|| c == 'L'
		|| c == 'T'
		|| c == 'Z'
		|| c == 'C' )
	{
		n = nCiol / 16;
		m = nCiol % 16;
		sprintf( pstr, "%d].B%x",n,m);
	}else
		sprintf( pstr, "%d]",nCiol );
	return strOut;
#endif
}


void SetMotherLine( char* pstr,char bOut )
{
	g_motherLine = (char*)MEMNEW( strlen( pstr ) + 1 );
	strcpy( g_motherLine, pstr );
}

char* CompilePlusCoil( char* strCoil, int bIsPlus,FILE* fout )
{
	char* strCFormat;
	char* strZ;
	char* strZFormat;
	char strL[5];
	char* strLFormat;
	char buf[251];

	
	sprintf(strL,"L%03d", g_nstatic++ );
	strLFormat = FormatCoil(strL);
	strCFormat = FormatCoil(strCoil);
	strZ = (char*)MEMNEW(5);
	sprintf( strZ,"Z%03d",g_ntmp++);
	strZFormat = FormatCoil(strZ);
	sprintf( buf,"if(%c%s&%c%s) %s=1;else %s=0;%s=%s;\n",
		bIsPlus ? '!' : ' ',strLFormat,bIsPlus  ? ' ' : '!',strCFormat,
		strZFormat,strZFormat,strLFormat,strCFormat );
	fwrite(buf,1,strlen(buf),fout);
	MEMFREE(strCFormat);
	MEMFREE(strLFormat);
	MEMFREE(strZFormat);
	return strZ;
}
char* CompileBlock( FILE* fout )	//编译成复合运算
{
	int bPair = 1;
	char* pres;
	char insBlock[512];
	int nblock = 0;
	char* pstr;
	char* pstr2;
	char c;
	while( slListGetLength( &g_lstOP ) )
	{
		c = (char)slListPop( &g_lstOP,0 );
		if( c == 0 )
			return NULL;
		if( c == ')' && bPair )
			bPair = 0;
		else if( c == '(' && !bPair )
			bPair = 1;
		else if( c == '!' )
		{
			insBlock[nblock++] = c;
			continue;
		}else if( c != '|' && c != '&' )
			return NULL;
		insBlock[nblock++] = c;
		if( c == '(' )
			continue;
		pstr = (char*)slListPop( &g_lstNum, NULL );
		if ( pstr == NULL )
			return NULL;
		//注意这里是反向保存，方便弹出的时候操作
		pstr2 = pstr + strlen( pstr ) - 1;
		while( pstr2 >= pstr )
			insBlock[nblock++] = *(pstr2--);
		MEMFREE(pstr);
	}
	if ( bPair )
	{
		insBlock[nblock] = 0;
		pres = (char*)MEMNEW( nblock + 1 );
		pstr = insBlock;
		pres[nblock--] = 0;
		while( *pstr != 0 )
		{
			pres[nblock--] = *(pstr++);
		}
		return pres;
	}else
		return NULL;
}

int SaveBlock( FILE* fout )
{
	char* pBlock;
	if( slListGetLength( &g_lstOP ) )	//不为空保存到指令块g_lstBlock
	{
		slListPush(&g_lstOP,(void*)')');
		pBlock = CompileBlock( fout );
		if( pBlock == NULL )
			return 0;
		
		slListPush( &g_lstBlock, pBlock );
		if( slListGetLength( &g_lstNum ) || slListGetLength( &g_lstOP ) )
			return 0;
	}
	return 1;
}

static int InsLineCompile( char* pList[], int szlist, FILE* fout )
{
	LPInsItem pins;
	int n;
	int n2;
	char* pBlock;		//指令块
	char* pBlock2;
	char* pBlock3;
	char* pstr;			//中间处理
	char* pstr2;
	char* pstr3;
	char* pformat;		//格式化后的线圈
	char* pformat2;
	char c;
	char insBuf[512];

	if( szlist <= 0 )
		return 0;
	pstr = pList[0];
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
	pins = GetInsItem( pstr );
	if ( pins->ins >= insLD && pins->ins <= insLDF )
	{
		if( !SaveBlock( fout ) )
			return 0;
		if(g_btmpset)
		{
			g_btmpset = 0;
			g_ntmp = 0;
		}
		slListPush(&g_lstOP,(void*)'(');
		if( g_Mc != NULL )
		{
			pformat = FormatCoil( g_Mc->n );
			slListPush(&g_lstNum,pformat);
			slListPush(&g_lstOP,(void*)'&');
		}
		if ( pins->ins == insLDI )
		{
			slListPush(&g_lstOP,(void*)'!');
		}
		pBlock = pList[1];
		if( pins->ins == insLDF || pins->ins == insLDP )
			pBlock = CompilePlusCoil(pBlock,(pins->ins == insLDP),fout);
		pformat = FormatCoil(pBlock);
		if( pins->ins == insLDF || pins->ins == insLDP )
			MEMFREE(pBlock);
		slListPush(&g_lstNum,pformat);
	}else if( pins->ins >= insAND && pins->ins <= insORF )
	{
		if( pins->ins == insAND 
			|| pins->ins == insANI 
			|| pins->ins == insANDP 
			|| pins->ins == insANDF )
			c = '&';
		else
			c = '|';
		slListPush( &g_lstOP, (void*)c );
		if ( pins->ins == insANI
			|| pins->ins == insORI )
			slListPush(&g_lstOP,(void*)'!');
		pstr = pList[1];
		if( pins->ins == insANDP || pins->ins == insANDF ||  pins->ins == insORP || pins->ins == insORF )
			pstr = CompilePlusCoil(pstr,(pins->ins == insANDP || pins->ins == insORP ),fout);
		pformat = FormatCoil(pstr);
		if( pins->ins == insANDP || pins->ins == insANDF ||  pins->ins == insORP || pins->ins == insORF )
			MEMFREE(pstr);
		slListPush(&g_lstNum,pformat);
	}else if( pins->ins >= insANB && pins->ins <= insORB )
	{
		if( !SaveBlock( fout ) )
			return 0;
		pBlock = (char*)slListPop( &g_lstBlock,NULL);
		pBlock2 = (char*)slListPop( &g_lstBlock,NULL);
		if( !pBlock2 ) pBlock2 = g_motherLine;
		if( pBlock == NULL || pBlock2 == NULL )
		{
			if( pBlock )
				MEMFREE( pBlock );
			if( pBlock2 )
				MEMFREE( pBlock2 );
			return 0;
		}
		pBlock3 = (char*)MEMNEW( strlen( pBlock ) + strlen( pBlock2 ) + 4);
		sprintf(pBlock3,"(%s%c%s)",pBlock2,( pins->ins == insANB ) ? '&' : '|',pBlock );
		slListPush( &g_lstBlock, pBlock3);
		if( pBlock )MEMFREE( pBlock );
		if( pBlock2 )MEMFREE( pBlock2 );
	}else if( pins->ins == insMPS )
	{
		g_btmpset = 0;
		if( !SaveBlock( fout ) )
			return 0;
		pBlock2 = (char*)slListPop( &g_lstBlock, NULL );
		if( pBlock2 == NULL )
			pBlock = g_motherLine;
		else
			pBlock = pBlock2;
		if( pBlock == NULL )
			return 0;
		pstr = (char*)MEMNEW( 5 );
		*pstr = 'Z';
		sprintf(pstr + 1, "%03d", g_ntmp++ );
		pformat = FormatCoil(pstr);
		sprintf(insBuf,"%s=%s;\n",pformat,pBlock);
		fwrite( insBuf, 1, strlen(insBuf), fout );
		slListPush( &g_lstBlock, pformat );
		slListPush( &g_lstStr,FormatCoil(pstr));
		MEMFREE(pBlock2);
		MEMFREE(pstr);
	}else if( pins->ins == insMRD || pins->ins == insMPP )
	{
		if( pins->ins == insMRD )
		{
			pstr = (char*)slListGetHeapTop( &g_lstStr, NULL );
			if( pstr )
			{
				pBlock = (char*)MEMNEW( strlen(pstr) + 1 );
				strcpy( pBlock, pstr );
			}else
				pBlock = NULL;
		}
		else
			pBlock = (char*)slListPop( &g_lstStr, NULL );
		if( pBlock == NULL )
			return 0;
		pBlock2 = (char*)slListPop( &g_lstBlock, NULL );
		MEMFREE(pBlock2);
		slListPush( &g_lstBlock,pBlock);
	}else if( pins->ins >= insOUT && pins->ins <= insMCR )
	{
		if( !SaveBlock( fout ) )
			return 0;
		pBlock2 = (char*)slListPop( &g_lstBlock, NULL );
		if( pBlock2 == NULL ) 
			pBlock = g_motherLine;
		else
		{
			g_btmpset = ( slListGetLength( &g_lstStr ) == 0 );
			pBlock = pBlock2;
			MEMFREE(g_motherLine);
			g_motherLine = NULL;
		}
		if( pBlock == NULL )
			return 0;
		if( g_motherLine == NULL )
		{
			pstr3 = NULL;
			pstr = pBlock;
			while( *pstr != 0 && *pstr != '&' && *pstr != '|' ) pstr++;
			if ( *pstr != 0 )	//存在逻辑
			{
				pstr2 = (char*)MEMNEW(5);
				sprintf( pstr2, "Z%03d", g_ntmp++ );
				pstr3 = FormatCoil(pstr2);
				sprintf( insBuf, "%s=%s;", pstr3,pBlock );
				fwrite( insBuf, 1, strlen(insBuf), fout );
				pBlock = pstr3;
				MEMFREE(pstr2);
			}
			SetMotherLine(pBlock,1);	//保存母线，因为在GX软件转换的PLC指令insOUT后没有insMRD
			MEMFREE(pstr3);
		}
		pBlock = g_motherLine;
		pformat = FormatCoil( pList[1] );
		if( pins->ins == insOUT )
		{
			if( szlist == 2 )
			{
				sprintf( insBuf, "%s=%s;\n", pformat,pBlock );
				fwrite( insBuf, 1, strlen(insBuf), fout );
			}else if( szlist == 3 )
			{
				if( *pList[1] == 'T' )
				{
					n = atoi( pList[1] + 1 );
					n2 = atoi( pList[2] + 1 );
					sprintf(insBuf,"if(%s){if(!p->V[%d].EN&!%s){p->V[%d].EN=1;p->V[%d].R=%d;p->V[%d].N=%d;}}else{p->V[%d].C=p->V[%d].EN=%s=0;}\n",
							pBlock,n,pformat,n,n,( *pList[2] == 'D') ? 1 : 0,n,n2,n,n,pformat);
					fwrite(insBuf,1,strlen(insBuf),fout);
				}else if( *pList[1] == 'C' )
				{
					n = atoi( pList[1] + 1 );
					n2 = atoi( pList[2] + 1 );
					pstr = (char*)MEMNEW(5);
					sprintf(pstr,"Z%03d",g_ntmp++);
					pstr2 = FormatCoil(pstr);

					sprintf( insBuf, "%s=%s;\n", pstr2,pBlock );
					fwrite( insBuf, 1, strlen(insBuf), fout );

					MEMFREE(pstr2);
					pstr2 = CompilePlusCoil(pstr,1,fout);

					pstr3 = FormatCoil(pstr2);
					sprintf(insBuf,"if(%s&&p->W[%d]<%d){ p->W[%d]++; if(p->W[%d]>=%d) %s=1;}\n",
							pstr3,n,n2,n,n,n2,pformat );
					fwrite(insBuf,1,strlen(insBuf),fout);

					MEMFREE(pstr3);
					MEMFREE(pstr2);
					MEMFREE(pstr);
				}
			}
		}else if( pins->ins == insSET || pins->ins == insRST )
		{
			sprintf( insBuf, "if(%s) %s=%d;\n", pBlock,pformat,(pins->ins == insSET) );
			fwrite( insBuf, 1, strlen(insBuf), fout );
		}else if( pins->ins == insPLS || pins->ins == insPLF )
		{
			pstr2 = (char*)MEMNEW(5);
			sprintf( pstr2, "Z%03d", g_ntmp++ );
			pformat2 = FormatCoil(pstr2);
			sprintf( insBuf, "%s=%s;", pformat2,pBlock );
			fwrite( insBuf, 1, strlen(insBuf), fout );
			
			pBlock3 = CompilePlusCoil(pstr2,(pins->ins == insPLS ),fout);
			MEMFREE(pformat2);
			pformat2 = FormatCoil(pBlock3);
			sprintf( insBuf, "%s=%s;\n", pformat,pformat2 );
			fwrite( insBuf, 1, strlen(insBuf), fout );

			MEMFREE(pBlock3);
			MEMFREE(pstr2);
			MEMFREE(pformat2);
		}else if( pins->ins == insMC )
		{
			n = atoi( pList[1] + 1 );
			if( n < 0 || n > 7 )
				return 0;
			g_Mc = g_McBlock + n;
			strcpy( g_Mc->n, pList[1] );
			strcpy( g_Mc->m, pList[2] );
			pformat2 = FormatCoil(pList[2]);
			sprintf( insBuf, "%s=%s;%s=", pformat2,pBlock,pformat );
			fwrite( insBuf, 1, strlen(insBuf), fout );

			if( n > 0 )
			{
				if( *g_McBlock[n - 1].n = 0 )	//没有上个状态，出错
					return 0;
				pstr3 = FormatCoil(g_McBlock[n - 1].n);
				fwrite( pstr3,1,strlen(pstr3),fout );
				fwrite( "&", 1, 1, fout );
				MEMFREE(pstr3);
			}
			fwrite( pformat2,1,strlen(pformat2),fout );
			fwrite( ";\n", 1, 2, fout );
			MEMFREE(pformat2);
		}else if ( pins->ins == insMCR )
		{
			n = atoi( pList[1] + 1 );
			if( n < 0 || n > 7 )
				return 0;
			n--;
			if( n < 0 )
				g_Mc = NULL;
			else
				g_Mc = g_McBlock + n;
		}
		MEMFREE( pBlock2 );
		MEMFREE( pformat );
	}else if( pins->ins >= insINV && pins->ins <= insMEF )
	{
		if( !SaveBlock( fout ) )
			return 0;
		pBlock = (char*)slListPop( &g_lstBlock, NULL );
		if( pBlock == NULL )
			return 0;
		pstr = (char*)MEMNEW(5);
		sprintf(pstr,"Z%03d",g_ntmp++);
		pstr2 = FormatCoil(pstr);

		sprintf( insBuf, "%s=%c%s;\n", pstr2,( pins->ins == insINV ) ? '!' : ' ',pBlock );
		fwrite( insBuf, 1, strlen(insBuf), fout );

		if( pins->ins == insMEF || pins->ins == insMEP )
		{
			MEMFREE(pstr2);
			pstr2 = CompilePlusCoil(pstr,( pins->ins == insMEP ),fout);
		}
		slListPush( &g_lstBlock, pstr2 );
		MEMFREE(pBlock);
		MEMFREE(pstr);
	}else if( pins->ins == insNOP )
	{

	}else if( pins->ins == insEND )
	{
		if( !SaveBlock( fout ) )
			return 0;
		if( slListGetLength( &g_lstBlock) )	//如果不为空则有母线未连接到输出，出错
			return 0;
	}else
		return 0;
	return 1;
}

int ParsePLC(char* pstr, int sz,FILE* fout)
{
	int res = -1;
	int nLine = 0;
	int szlist;
	int n;
	char* pstrEnd = pstr + sz;
	char* pInsLine[10];		//最多10个
	fseek(fout,0,SEEK_SET);
	fwrite(STR_CHEAD,1,strlen(STR_CHEAD),fout);
	while( pstr < pstrEnd )
	{
		n = 0;
		szlist = GetInisLine( &pstr, pInsLine, 10,&n );
		nLine += n;
		if( szlist && InsLineCompile( pInsLine, szlist, fout ) == 0 )
		{
			printf("error instruction at line:%d\n", nLine );
			goto leave;
		}
	}
	res = 0;
leave:
	if( res == 0 )
		fwrite(STR_CTAIL,1,strlen(STR_CTAIL),fout);
	return res;
}

int initStr2c()
{
	initInsHash();
	slListInit( &g_lstBlock );
	slListInit( &g_lstOP );
	slListInit( &g_lstNum );
	slListInit( &g_lstStr );
	return 1;
}

void relaseStr2c()
{
	char* pstr;
	while( slListGetLength( &g_lstStr ) )
	{
		pstr = (char*)slListPop( &g_lstStr,NULL );
		MEMFREE( pstr );
	}
	while( slListGetLength( &g_lstBlock ) )
	{
		pstr = (char*)slListPop( &g_lstBlock,NULL );
		MEMFREE( pstr );
	}
	while( slListGetLength( &g_lstNum ) )
	{
		pstr = (char*)slListPop( &g_lstNum,NULL );
		MEMFREE( pstr );
	}
	if( g_motherLine )
		MEMFREE(g_motherLine);
}
int Str2c( LPPLCCData pData )
{
	int res;
	initStr2c();
	res = ParsePLC( (char*)pData->pSource,pData->szSource,pData->fout);
	relaseStr2c();
	return res;
}
