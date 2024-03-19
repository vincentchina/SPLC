#ifndef __MEMCHECKER_H__
#define __MEMCHECKER_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>


void InitializeMemChecker( FILE* fp );
void* AllocMemery(unsigned long dwBytes,unsigned long dwLine,const char* lpFileName);
void* ReAllocMemery( void* mem, unsigned long dwBytes,unsigned long dwLine,const char* lpFileName );
void FreeMemery(void* lpMen );
int MemCheck();

#ifdef _DEBUG

#define MEMNEW(dwBytes) \
	AllocMemery((unsigned long)dwBytes,__LINE__,__FILE__)

#define MEMFREE(lpMem) \
	FreeMemery(lpMem)

#define MEMRENEW(mem,dwBytes) \
	ReAllocMemery((void*)mem,(unsigned long)dwBytes,__LINE__,__FILE__)

#define MEMCHECK	\
	MemCheck()

#else
#define MEMNEW(dwBytes) \
	malloc(dwBytes)

#define MEMFREE(lpMem) \
	free(lpMem)

#define MEMRENEW(mem,dwBytes) \
	realloc( (void*)mem,(unsigned long)dwBytes )

#define MEMCHECK

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __MEMCHECKER_H__ */
