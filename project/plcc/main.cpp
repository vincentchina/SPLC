#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include "Communication.h"
#include "plcc.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#endif*/

#include "common\misc.h"
#include "plcInsSys\plcInsSys.h"

#ifdef _DEBUG
#pragma comment( lib, "..\\..\\Debug\\libx" )
#else
#pragma comment( lib, "..\\..\\Release\\libx" )
#endif
void RevCallBack( const unsigned char* buf, int sz, void* userData )
{

}

//remember to delete the return value
static wchar_t* ConvertToWchar( char* pstr )
{
	wchar_t* pwstr;
	if( pstr == NULL )
		return NULL;

	int nLength = strlen(pstr);
	int nLen = MultiByteToWideChar( CP_ACP, 0, pstr, nLength, NULL, 0 );
	pwstr = new wchar_t[ nLen + 1 ];
	MultiByteToWideChar( CP_ACP, 0, pstr, nLength, pwstr, nLen );
	pwstr[ nLen ] = 0;
	return pwstr;
}

static const char g_version[]="v2.0.1b";
static const char g_strhelp[]="KingStrong PLC Compiler              v2.0.1b\n\
Tools Copyright (c) 1991-2015 xhs\n\
\n\
Usage: plcc.exe [-options] filename\n\
\n\
Option arguments surrounded by [] declare they are optional.  Option arguments\n\
separated by commas specify that the option's argument must be chosen from\n\
those arguments listed.\n\
\n\
General Options:\n\
-o filename   output the file name\n\
-h            show this help text\n\
-v            show version\n\
-p            specified the input file is WPG\n\
-e addr       specified the WPG's enter addr\n\
-t filename   specified the WPG's temp file\n\
-x            output the hex string\n\
-d port       download to the specified com port\n\
-D port       download only\n\
-DEBUG        DEBUG mode\n";
/*-c            compile to c code\n\*/

typedef struct tagCMDData
{
	const char*		fileName;
	const char*		outputFile;
	const char*		wpgTmpFile;
	PLCCData		plccData;
	CCommunication	communication;
	CDevUart		uart;
	CDevPip			Pip;
	CDevSocket		socket;
	UINT			bDownloadOnly:1;
}CMDData,*LPCMDData;

int Downloader( LPCMDData pData,FILE* fPrg )
{
	//先下载程序
	int bRes;
	fseek( fPrg, 0, SEEK_END );
	size_t len = ftell( fPrg );
	unsigned char* pProgram = new unsigned char[ len ];
	if( pProgram == NULL ) return 0;
	fseek( fPrg, 0, SEEK_SET );
	fread( pProgram, sizeof( unsigned char ), len, fPrg );
	bRes = pData->communication.Download( pProgram, len );
	if( !bRes )
		printf( "warning: Download ERROR!\n");

	delete[] pProgram;
	return !bRes;
}

#pragma warning( disable:4996 )
int ParseArg( LPCMDData pData, int argc, char** argv )
{
	char** parg;
	char** pargEnd;
	char* pstr;
	unsigned char hex;
	unsigned int addr;
	char straddr[11];
	char bUart;
	char strCom[MAX_PATH];
	char* paddrBegin;

	if( argc == 1 )
	{
		printf( g_strhelp );
		return 1;
	}
	pargEnd = argv + argc;
	parg = argv + 1;
	while( parg < pargEnd )
	{
		//printf( "%s\n",*parg );
		pstr = *parg;
		if( *pstr == '-' )
		{
			pstr++;
			switch( *pstr )
			{
			case 'v':
				printf( g_version);
				return 1;
				break;
			case 'o':
				if( ++parg >= pargEnd )
				{
					printf( "format error!\n");
					return -1;
				}
				if( !pData->outputFile )
					pData->outputFile = *parg;
				else
				{
					printf("format error!\n");
					return -1;
				}
				break;
			case 't':
				if( ++parg >= pargEnd )
				{
					printf("format error!\n");
					return -1;
				}
				if( !pData->wpgTmpFile )
					pData->wpgTmpFile = *parg;
				else
				{
					printf( "format error!\n");
					return -1;
				}
				break;
			case 'h':
				printf( g_strhelp);
				return 1;
				break;
			case 'p':
				pData->plccData.bWPG = 1;
				break;
			case 'e':
				if( ++parg >= pargEnd )
				{
					printf( "format error!\n");
					return -1;
				}
				strcpy_s( straddr,11,*parg );
				strupr( straddr );
				if( *straddr == '0' && *( straddr + 1 ) == 'X' )
					paddrBegin = straddr + 2;
				else
					paddrBegin = straddr;
				addr = 0;
				pstr = straddr + strlen( straddr ) - 1;
				while( pstr >= paddrBegin )
				{
					if( *pstr >= 'A' && *pstr <= 'F' )
						hex = *pstr - 55;
					else if( *pstr >= '0' && *pstr <= '9' )
						hex = *pstr - 48;
					else
					{
						printf( "addr format error!\n");
						return -1;
					}
					addr <<= 4;
					addr |= hex;
					pstr--;
				}
				pData->plccData.offsetWPG = addr;
				break;
			//case 'c':
			//	pData->plccData.bToCCode = 1;
			//	break;
			case 'x':
				pData->plccData.bToHex = 1;
				break;
			case 'd':
			case 'D':
				if( strcmp( pstr, "DEBUG" ) == 0 )
				{
					pData->plccData.bDebug = 1;
					break;
				}
				if( ++parg >= pargEnd )
				{
					printf( "format error!\n");
					return -1;
				}
				strcpy_s( strCom,MAX_PATH,*parg );
				if( *strCom == 'C' && strCom[1] == 'O' && strCom[2] == 'M'
					|| *strCom == 'c' && strCom[1] == 'o' && strCom[2] == 'm' )
					bUart = TRUE;
				else
					bUart = FALSE;
				if( pData->communication.GetDev() )
					pData->communication.GetDev()->Close();
				{
					wchar_t* wstr = ConvertToWchar( strCom );
					if( wstr )
					{
						if( bUart )
						{
							COMCONFIG conf;
							memset( &conf, 0, sizeof( conf ) );
							wcscpy( conf.comName, wstr );
							conf.dwBaudRate = 19200;
							conf.bitByteSize = 8;
							conf.bitParity = NOPARITY;
							conf.bitStopBits = ONESTOPBIT;
							conf.szBuff = 512;
							pData->uart.Config( &conf );
							pData->communication.SetSelDev( &pData->uart );
						}else
						{
							if( pData->Pip.Config( wstr,100 ) )
								pData->communication.SetSelDev( &pData->Pip );
						}
						delete[] wstr;
					}
				}
				if( pData->communication.GetDev() )
					pData->communication.SetRevCallBack( RevCallBack );
				else
				{
					printf( "warning: can not open PORT!\n");
				}
				pData->bDownloadOnly = ( *pstr == 'D' );
				break;
			default:
				printf( "invalid arg:%s\n",*parg);
				return -1;
			}
		}else
		{
			if( !pData->fileName )
				pData->fileName = *parg;
			else
			{
				printf( "format error!\n");
				return -1;
			}
		}
		parg++;
	}
	if( pData->fileName == NULL )
	{
		printf( "no input file!\n");
		return -1;
	}
	return 0;
}

int GetArgv( char* pCmdLine, char* pArgv[], int sz )
{
	int nArgc = 0;
	char** pArgGet;
	char bQuota = 0;
	char** pArgvEnd;

	pArgGet = pArgv;
	pArgvEnd = pArgGet + sz;
	while( pArgGet < pArgvEnd && *pCmdLine != '\0' )
	{
		//skip space
		while( *pCmdLine != '\0' && ( *pCmdLine == ' ' || *pCmdLine == '\t' ) ) ++pCmdLine;

		if( *pCmdLine == '\0' )
			break;
		bQuota = ( *pCmdLine == '\"' );
		if( bQuota )
			++pCmdLine;
		*pArgGet = pCmdLine;
		++nArgc;
		while( *pCmdLine != '\0' )
		{
			if( bQuota )
			{
				if(  *pCmdLine == '\"' )
					break;
			}else
			{
				if(  *pCmdLine == ' ' )
					break;
			}
			++pCmdLine;
		}
		if( *pCmdLine == '\0' )
			break;
		*(pCmdLine++) = '\0';
		++pArgGet;
	}
	return nArgc;
}


int _tmain(int argc, _TCHAR* argv[]) 
{
	int res;
	FILE* fin = NULL;
	FILE* fout = NULL;
	char* pstr;
	int nlen;
	long szIn;
	char* err;
	char* outputFile = NULL;
	char* wpgTmpFile = NULL;
	char* selOutput = NULL;

	char* inbuf = NULL;
	char* outbuf = NULL;	

	CMDData cmdData;
	memset( &cmdData, 0, sizeof(cmdData) );
	if( ( res = ParseArg( &cmdData, argc, argv ) ) == 0 )
	{
		szIn = OpenPLCFile( &fin,cmdData.fileName,&err );
		if( szIn == -1 )
		{
			printf( "can not open file \"&s\"Err: %s\n",cmdData.fileName,err);
			res = -1;
			goto leave;
		}
		//先处理download only
		if( cmdData.bDownloadOnly )
		{
			res = Downloader( &cmdData,fin );
			goto leave;
		}

		inbuf = new char[ szIn + 1 ];
		if( fread(inbuf,1,szIn,fin) != szIn )
		{
			printf( "read file error!\n");
			res = -1;goto leave;
		}
		inbuf[szIn] = 0;
		if( cmdData.plccData.bWPG )
		{
			if( cmdData.wpgTmpFile )
				selOutput = (char*)cmdData.wpgTmpFile;
			else
			{
				nlen = strlen(cmdData.fileName);
				wpgTmpFile = new char[ nlen + 6 ];
				strcpy(wpgTmpFile,cmdData.fileName);
				pstr = wpgTmpFile + nlen;
				while( *pstr != '.' && pstr != wpgTmpFile )	pstr--;
				if( pstr == wpgTmpFile )
					pstr = wpgTmpFile + nlen;
				strcpy( pstr,".code");
				selOutput = wpgTmpFile;
			}
			if ( ( fout = fopen( selOutput,"wb+" ) ) == NULL )
			{
				printf("can not create file: %s\n",selOutput);
				res = -1;goto leave;
			}
			cmdData.plccData.pSource = (unsigned char*)inbuf;
			cmdData.plccData.szSource = szIn;
			cmdData.plccData.fout = fout;
			cmdData.plccData.offsetWPG = 0x44;
			if( ( res = Wpg2Str( &cmdData.plccData ) ) != 0 )
			{
				goto leave;
			}
			delete[] inbuf;
			fflush( fout );
			fseek( fout, 0, SEEK_END );
			szIn = ftell(fout);
			inbuf = new char[ szIn + 1 ];
			fseek( fout, 0, SEEK_SET );
			//szIn = fread(inbuf,1,szIn,fout);
			if( fread(inbuf,1,szIn,fout) != szIn )
			{
				printf("read file error!\n");
				res = -1;goto leave;
			}
			fclose(fout);
			fout = NULL;
			inbuf[szIn] = 0;
		}
		if( cmdData.outputFile == NULL )
		{
			nlen = strlen(cmdData.fileName);
			outputFile = new char[ nlen + 5 ];
			strcpy(outputFile,cmdData.fileName);
			pstr = outputFile + nlen;
			while( *pstr != '.' && pstr != outputFile )	pstr--;
			if( pstr == outputFile )
				pstr = outputFile + nlen;
			strcpy( pstr,".out");
			selOutput = outputFile;
		}else
			selOutput = (char*)cmdData.outputFile;
		fout = fopen( selOutput,"wb+" );
		if( fout == NULL )
		{
			printf("can not create file: %s\n",selOutput);
			res = -1;goto leave;
		}

		//begin compile
		cmdData.plccData.pSource = (unsigned char*)inbuf;
		cmdData.plccData.szSource = szIn;
		cmdData.plccData.fout = fout;
		//if( cmdData.plccData.bToCCode )
		//	res = Str2c( &cmdData.plccData );
		//else
		{
			res = Str2Bin( &cmdData.plccData );
		}
		//decode
		/*if( res == 0 )
		{
			delete[] inbuf;
			fflush( fout );
			fseek( fout, 0, SEEK_END );
			szIn = ftell(fout);
			inbuf = new char[ szIn + 1 ];
			fseek( fout, 0, SEEK_SET );
			//szIn = fread(inbuf,1,szIn,fout);
			if( fread(inbuf,1,szIn,fout) != szIn )
			{
				printf("read file error!\n");
				res = -1;goto leave;
			}
			fclose(fout);
			fout = NULL;
			inbuf[szIn] = 0;
			fout = fopen( "bin2str.txt","wb+" );
			if( fout == NULL )
			{
				printf("can not create file: %s\n",selOutput);
				res = -1;goto leave;
			}
			cmdData.plccData.pSource = (unsigned char*)inbuf;
			cmdData.plccData.szSource = szIn;
			cmdData.plccData.fout = fout;
			res = Bin2Str( &cmdData.plccData );
		}*/
		//begin download if specified
		if( res == 0 && cmdData.communication.GetDev() )
			res = Downloader( &cmdData,fout );
	}
leave:
	if( fin )
		fclose(fin);
	if( fout )
		fclose(fout);
	if( wpgTmpFile )
		delete[] wpgTmpFile;
	if( outputFile )
		delete[] outputFile;
	if( inbuf )
		delete[] inbuf;
	if( outbuf )
		delete[] outbuf;
	CBaseDev* pDev = cmdData.communication.GetDev();
	if( pDev )
		pDev->Close();
	return res;
}
#pragma warning( default:4996 )
