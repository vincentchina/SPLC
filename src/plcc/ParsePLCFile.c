#include <stdio.h>
#include <stdlib.h>


//#include "ParsePLCFile.h"
#define MAX_PLCSOURCE_SIZE	( 1024*1024*1 )		//1m

static __inline void SkipLine( char** pstr )
{
	while( **pstr != 0 && **pstr != '\n' ) 
	{
		if( **pstr == '\r' ) **pstr = 0;
		(*pstr)++;
	}
	if( **pstr != 0 )
		**pstr = 0,(*pstr)++;
}
int GetInisLine(char** pstr, char* pList[], int szlist, int* plineGet )
{
	enum{
		GETINIS_BEGINE,
		GETINIS_VALUDE,
		GETINIS_SKIPVALUDE,
	};
	int nline = 0;
	int nlst = 0;
	int stata = GETINIS_BEGINE;
	if( szlist <= 0 )
		return 0;
	memset( pList, 0, sizeof(char*) * szlist );
	while( **pstr != 0 )		//skip space;
	{
		if( **pstr == '\t' || **pstr == ' ' || **pstr == '\r' ) (*pstr)++;
		else if ( **pstr == '\n' ) (*pstr)++,nline++;
		else if (**pstr == ';')//遇到注释，跳过整行
		{
			pList[ nlst++ ] = *pstr;
			SkipLine( pstr );
			nline++;
			goto leave;
		}
		else break;
	}
	while( **pstr != 0 )
	{
		if( stata == GETINIS_VALUDE )
		{
			if( nlst < szlist )
				pList[nlst++] = *pstr;
			else
				printf("warning: no more space to get INIS line\n");
			if( **pstr == ';')//遇到注释，跳过整行
			{
				SkipLine( pstr );
				nline++;
				goto leave;
			}
			stata = GETINIS_SKIPVALUDE;
			(*pstr)++;
		}else if( stata == GETINIS_SKIPVALUDE )
		{
			if( **pstr == '\t' || **pstr == ' ' || **pstr == '\r' )
			{
				**pstr = 0;
				stata = GETINIS_BEGINE;
			}
			else if( **pstr == '\n' )
			{
				**pstr = 0;(*pstr)++; ++nline; goto leave;
			}
			(*pstr)++;
		}else if ( stata == GETINIS_BEGINE )
		{
			if( **pstr == '\t' || **pstr == ' ' || **pstr == '\r' )
			{
				**pstr = 0;
				(*pstr)++;
			}
			else if( **pstr == '\n' )
			{
				**pstr = 0;(*pstr)++; ++nline; goto leave;
			}
			else
				stata = GETINIS_VALUDE;
		}
	}
leave:
	if( plineGet )
		*plineGet = nline;
	return nlst;
}

long OpenPLCFile( FILE** pFile, const char* plcFile, char** err )
{
	long szIn;
	*pFile = fopen(plcFile,"rb");
	if( *pFile == NULL )
	{
		err && ( *err = "can not open file" );
		return -1;
	}
	fseek(*pFile,0,SEEK_END);
	szIn = ftell(*pFile);
	if( szIn > MAX_PLCSOURCE_SIZE )
	{
		err && ( *err = "the input file is too large!" );
		fclose(*pFile);
		*pFile = NULL;
		return -1;
	}
	fseek(*pFile,0,SEEK_SET);
	return szIn;
}

long OpenPLCFileW( FILE** pFile, const wchar_t* plcFile, wchar_t** err )
{
	long szIn;
	*pFile = _wfopen(plcFile,L"rb");
	if( *pFile == NULL )
	{
		err && ( *err = L"can not open file" );
		return -1;
	}
	fseek(*pFile,0,SEEK_END);
	szIn = ftell(*pFile);
	if( szIn > MAX_PLCSOURCE_SIZE )
	{
		err && ( *err = L"the input file is too large!" );
		fclose(*pFile);
		*pFile = NULL;
		return -1;
	}
	fseek(*pFile,0,SEEK_SET);
	return szIn;
}
