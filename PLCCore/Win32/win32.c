#include "PLCCore.h"
#include "MemManager.h"
#include "insClass.h"

# pragma warning (disable:4996)

void M_DumpMem( PLPLCSystem psys )
{
	FILE* fb;
	fb = fopen( "mem.dump","wb" );
	if( fb == NULL )
		return;
	fwrite( psys->pBitMap, sizeof(uint16), psys->nBitMapsz, fb );
	fwrite( psys->pDataMap, sizeof(uint16), psys->nDataMapsz, fb );
	fwrite( psys->R[REG_D].pData, sizeof( uint16 ),psys->R[REG_D].sz, fb );
	fclose( fb );
}

void M_dDump( mbyte* pdata, uint16 sz )
{
	FILE* fb;
	fb = fopen( "c:\\program.dump","wb" );
	if( fb == NULL )
		return;
	fwrite( pdata, sizeof(mbyte), sz, fb );
	fclose( fb );
}

//用于32位乘法
uint16 g_mun1[2],g_mun2[2],g_result[4];
uint16 g_tmp1[3];
uint16 g_tmp2[3];

void M_MulNum32( uint32 num1, uint32 num2, uint32* resultH, uint32* resultL )
{
	int i,j;
	uint32* p32 = (uint32*)g_mun1;
	*p32 = num1;
	p32 = (uint32*)g_mun2;
	*p32 = num2;
	p32 = (uint32*)g_result;
	*p32++ = 0;
	*p32++ = 0;
	g_tmp1[0] = g_mun1[0];
	g_mun1[0] = g_mun1[1];
	g_mun1[1] = g_tmp1[0];
	g_tmp1[0] = g_mun2[0];
	g_mun2[0] = g_mun2[1];
	g_mun2[1] = g_tmp1[0];
	g_tmp1[0] = 0;
	g_tmp1[1] = 0;
	g_tmp1[2] = 0;
	g_tmp2[0] = 0;
	g_tmp2[1] = 0;
	g_tmp2[2] = 0;
__asm{
;Step1:
	mov ax,g_mun1+2
	mov bx,g_mun2+2
	mul bx
	mov g_tmp1+4,ax
	mov cx,dx
	;-------Number A 的低16位乘以 Number B 的低十六位,结果存入g_tmp1+4
	;进位存入CX
	mov ax,g_mun1
	mov bx,g_mun2+2
	mul bx
	mov g_tmp1+2,ax
	add g_tmp1+2,cx
	adc g_tmp1,dx
	;-------Number A 的高16位乘以 Number B 的低十六位,结果存入g_tmp1+4
	;结果存入g_tmp1+2,将g_tmp1+2与上一次相乘的进位相加存入g_tmp1+2
	;将第二次相乘的进位带符号相加入 g_tmp1(前一位的相加可能产生进位)
;Step2:
	mov ax,g_mun1+2
	mov bx,g_mun2
	mul bx
	mov g_tmp2+4,ax
	mov cx,dx
	;-------同理
	mov ax,g_mun1
	mov bx,g_mun2
	mul bx
	mov g_tmp2+2,ax
	add g_tmp2+2,cx
	adc g_tmp2,dx
	;-------同理
;SUM:
	mov ax,g_tmp1+4
	mov g_result+6,ax

	mov ax,g_tmp2+4
	add g_tmp1+2,ax
	mov ax,g_tmp1+2
	mov g_result+4,ax

	mov ax,g_tmp2+2
	adc g_tmp1,ax
	mov ax,g_tmp1
	mov g_result+2,ax

	mov ax,g_tmp2
	adc g_result,ax
	;-------错位相加
	}
	for ( i = 0, j = sizeof( g_result ) / sizeof( uint16 ) - 1; i < j; ++i,--j )
	{
		g_tmp1[0] = g_result[i];
		g_result[i] = g_result[j];
		g_result[j] = g_tmp1[0];
	}
	p32 = (uint32*)g_result;
	*resultL = *p32++;
	*resultH = *p32;
}

#pragma warning(default:4996)
