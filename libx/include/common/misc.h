#ifndef __MISC_H__
#define __MISC_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "dllpreHead.h"

unsigned long LIBX_API HashByte( const char* buf, int nlen );
unsigned long LIBX_API HashStr( const char* str );
unsigned long LIBX_API Hash2Index( unsigned long hash, unsigned long maxindex );

int LIBX_API Str2Num( char* str, unsigned int* pnum, int base );
int LIBX_API Num2Str( unsigned int num, int base, char* str, int sz, char* fill );
int LIBX_API OStr2Num( char* strO,unsigned int* pnum );
unsigned int LIBX_API DStr2Num( char* strD,unsigned int* pnum  );

#ifdef __cplusplus
}
#endif
#endif	
