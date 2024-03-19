#ifndef __WIN32_H__
#define __WIN32_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long uint64;
typedef long long int64;
typedef short api_res;
typedef unsigned char mbyte;

#define M_GetData8( p )		( *p )
#define M_SkipData8( p )	( ++(p) )

#define M_GetData16( p )	( *( (uint16*)p ) )
#define M_SkipData16( p )	( (p) += 2 )

#define M_GetData32( p )	( *( (uint32*)p ) )
#define M_SkipData32( p )	( (p) += 4 )

#define _DPRINTF( ... ) printf( __VA_ARGS__ )

void M_MulNum32( uint32 num1, uint32 num2, uint32* resultH, uint32* resultL );

#ifdef __cplusplus
}
#endif
#endif
