#include "Leakchecker.h"

#ifdef _DEBUG       // most of this file is for debugging 

/* Memory block identification */ 
#define _FREE_BLOCK      0 
#define _NORMAL_BLOCK    1 
#define _CRT_BLOCK       2 
#define _IGNORE_BLOCK    3 
#define _CLIENT_BLOCK    4 
#define _MAX_BLOCKS      5 

void* __cdecl operator new(size_t nSize, int nType, LPCSTR lpszFileName, int nLine); 
void* __cdecl operator new[](size_t nSize, int nType, LPCSTR lpszFileName, int nLine); 


#ifndef _AFX_NO_DEBUG_CRT 

void* __cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine) 
{ 
	return ::operator new(nSize, _NORMAL_BLOCK, lpszFileName, nLine); 
} 

void* __cdecl operator new[](size_t nSize, LPCSTR lpszFileName, int nLine) 
{ 
	return ::operator new[](nSize, _NORMAL_BLOCK, lpszFileName, nLine); 
} 

void __cdecl operator delete(void* pData, LPCSTR /* lpszFileName */, 
	int /* nLine */) 
{ 
	::operator delete(pData); 
} 

void __cdecl operator delete[](void* pData, LPCSTR /* lpszFileName */, 
	int /* nLine */) 
{ 
	::operator delete(pData); 
} 


void* __cdecl operator new[](size_t nSize, int nType, LPCSTR lpszFileName, int nLine) 
{ 
	return ::operator new(nSize, nType, lpszFileName, nLine); 
} 

#endif // _AFX_NO_DEBUG_CRT 
#endif // _DEBUG