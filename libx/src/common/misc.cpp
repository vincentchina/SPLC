#include "common/misc.h"
#include <string.h>

//hash
static const unsigned long g_magicKey = 3559;

unsigned long HashByte( const char* buf, int nlen )
{
	unsigned long res = 0;
	char* pb,*pend;

	pb = (char*)buf;
	pend = pb + nlen;

	while( pb < pend )
		res += ( ( res << 4 ) ) + ( *pb++ ^ g_magicKey );

	return res;
}

unsigned long HashStr( const char* str )
{
	return HashByte( str, strlen(str) );
}

unsigned long Hash2Index( unsigned long hash, unsigned long maxindex )
{
	unsigned long m;
	while( ( hash / maxindex ) != 0 )
	{
		m = hash % maxindex;
		hash /= maxindex;
		hash += m;
	}
	return hash;
}


char g_strBase[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G'};

int Str2Num( char* str, unsigned int* pnum, int base )
{
	unsigned int m;
	unsigned int tmp;
	int res = 0;
	char* pstrBegin = str;
	int baseTmp;
	char* pstr;
	if( base - 1 > sizeof(g_strBase) || *str == '\0' )
		return 0;
	pstr = str + strlen(str) - 1;;
	baseTmp = ( ( base > 10 ) ? 10 : base );
# pragma warning (disable:4996)
	_strupr(str);
# pragma warning (default:4996)
	m = 1;
	if( *pstrBegin == '-' )
	{
		if( base != 10 )
			return 0;
		++pstrBegin;
	}
	while( pstr >= pstrBegin )
	{
		if( *pstr >= g_strBase[0] && *pstr < g_strBase[baseTmp] )
		{
			tmp = m * ( *pstr - '0' );
			res += tmp;
			m *= base;
		}else if( *pstr >= g_strBase[baseTmp] && *pstr < g_strBase[base] )
		{
			tmp = m * ( ( *pstr - 'A' ) + baseTmp );
			res += tmp;
			m *= base;
		}else
			return 0;
		pstr--;
	}
	if( *str == '-' )
		res = -res;
	if( pnum )
		*pnum = res;
	return 1;
}
int Num2Str( unsigned int num, int base, char* str, int sz, char* fill )
{
	char* p1,*p2;
	char strnum[33];
	char* pc = strnum + 32;
	*pc = 0;
	if( base > sizeof(g_strBase) )
		return 0;
	while( pc > strnum )
	{
		*pc = g_strBase[num % base];
		if( ( num /= base ) == 0 )
			break;
		--pc;
	}
	if( num != 0 || pc - strnum > sz )
		return 0;
	p1 = strnum + 32;
	p2 = fill ? str + sz - 1 : str + ( pc - strnum );
	while( p1 >= pc )
		*p2-- = *p1--;
	if( fill )
	{
		pc = fill;
		p1 = str;
		while( p1 < p2 )
		{
			*p1++ = *pc;
			if( *pc == 0 )
				pc = fill;
			else
				pc++;
		}
	}
	return 1;
}

int OStr2Num( char* strO,unsigned int* pnum )
{
	return Str2Num( strO, pnum, 8 );
}

unsigned int DStr2Num( char* strD,unsigned int* pnum  )
{
	return Str2Num( strD, pnum, 10 );
}

