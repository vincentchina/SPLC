#include "Memchecker.h"

typedef struct tagMemData
{
	void* mem;
	unsigned long dwLine;
	unsigned long dwBytes;
	const char* lpFileName;
	struct tagMemData* pNextNod;
}MemData,*LPMemData;

typedef struct tagMemChecker
{
	LPMemData pMemList;
	FILE* fp;
}MemChecker,*LPMemChecker;

static MemChecker g_memChecker={ NULL, NULL };

void InitializeMemChecker( FILE* fp )
{
	g_memChecker.fp =fp;
	g_memChecker.pMemList = NULL;
}

void FreeMemData( LPMemData* ppMem, int bFreeMem )
{
	if( ppMem == NULL )
		return;
	if( *ppMem != NULL )
	{
		if( (*ppMem)->mem != NULL && bFreeMem )
			free((*ppMem)->mem);
		free((*ppMem));
		*ppMem = NULL;
	}
}

void PutMemListNod( LPMemData pMemNew )
{
	LPMemData* ppMemNod;
	ppMemNod = &(g_memChecker.pMemList);
	while( *ppMemNod != NULL ) ppMemNod = &( (*ppMemNod)->pNextNod );
	*ppMemNod = pMemNew;
}

void DeleMemListNodByMem( void* mem, int bFreeMem )
{
	LPMemData* ppMemNod;
	LPMemData pMemNext;
	if( mem == NULL )
		return;
	ppMemNod = &(g_memChecker.pMemList);
	while( *ppMemNod != NULL )
	{
		pMemNext = (*ppMemNod)->pNextNod;
		if( (*ppMemNod)->mem == mem )
		{
			FreeMemData( ppMemNod, bFreeMem );
			*ppMemNod = pMemNext;
			break;
		}
		ppMemNod = &( (*ppMemNod)->pNextNod );
	}
}

void* AllocMemery(unsigned long dwBytes,unsigned long dwLine,const char* lpFileName)
{
	LPMemData pMemNew = NULL;
	pMemNew = (LPMemData)malloc(sizeof(MemData));
	if( pMemNew == NULL )
		return NULL;

	pMemNew->mem = malloc(dwBytes);
	if( pMemNew->mem == NULL )
		goto error;
	pMemNew->dwBytes = dwBytes;
	pMemNew->dwLine = dwLine;
	pMemNew->lpFileName = lpFileName;
	pMemNew->pNextNod = NULL;
	PutMemListNod( pMemNew );
	return pMemNew->mem;
error:
	FreeMemData( &pMemNew,1 );
	return NULL;
}

void* ReAllocMemery( void* mem, unsigned long dwBytes,unsigned long dwLine,const char* lpFileName )
{
	LPMemData pMemNew = NULL;
	DeleMemListNodByMem(mem,0);
	pMemNew = (LPMemData)malloc(sizeof(MemData));
	if( pMemNew == NULL )
		return NULL;
	pMemNew->mem = realloc(mem,dwBytes);
	if( pMemNew->mem == NULL )
		goto error;
	pMemNew->dwBytes = dwBytes;
	pMemNew->dwLine = dwLine;
	pMemNew->lpFileName = lpFileName;
	pMemNew->pNextNod = NULL;
	PutMemListNod( pMemNew );
	return pMemNew->mem;
error:
	FreeMemData( &pMemNew,1 );
	return NULL;
}

void FreeMemery(void* lpMen)
{
	DeleMemListNodByMem(lpMen,1);
}

int MemCheck()
{
	unsigned long i;
	unsigned char* pmem;
	FILE* fp;
	int bLeack = 0;
	LPMemData pMemNod;
	LPMemData pMemNext;
	pMemNod = g_memChecker.pMemList;
	bLeack = ( pMemNod != NULL );
	if( g_memChecker.fp == NULL )
		fp = stdout;
	else
		fp = g_memChecker.fp;
	if( bLeack )
	{
		fprintf(fp,"******************MemCheck******************\n");
		while( pMemNod != NULL )
		{
			fprintf(fp,"Memery leak at file: %s\r\n \tof line %ld \tbytes:%ld\tdata:",
				pMemNod->lpFileName,
				pMemNod->dwLine,
				pMemNod->dwBytes);
			pmem = (unsigned char*)pMemNod->mem;
			for ( i = 0; i < pMemNod->dwBytes; i++ )
			{
				//fprintf(fp,"%02x ",pmem[i]);
			}
			pMemNext = pMemNod->pNextNod;
			FreeMemData( &pMemNod,1 );
			pMemNod = pMemNext;
		}
		fprintf(fp, "\n");
	}
	return bLeack;
}

