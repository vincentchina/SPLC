#ifndef __WIN32_H__
#define __WIN32_H__

#ifdef __cplusplus
extern "C"{
#endif

typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;
typedef unsigned long uint64;
typedef long int64;
typedef short api_res;
typedef unsigned char mbyte;

#define NULL 0

#define M_GetData8( p )		( *p )
#define M_SkipData8( p )	( ++(p) )

#define M_GetData16( p )	( *( (uint16*)p ) )
#define M_SkipData16( p )	( (p) += 2 )

#define M_GetData32( p )	( *( (uint32*)p ) )
#define M_SkipData32( p )	( (p) += 4 )


#ifdef __cplusplus
}
#endif
#endif
